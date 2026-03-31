#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <title>STEM Car - Direct Control</title>
  <style>
    /* Estilo muito parecido com o original/NippleJS dark theme */
    body { text-align: center; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #1a1a1a; color: #fff; margin: 0; overflow: hidden; height: 100vh; display: flex; flex-direction: column; align-items: center; justify-content: center;}
    h2 { margin: 0 0 10px 0; font-size: 28px; color: #4CAF50; text-shadow: 0 2px 4px rgba(0,0,0,0.5); }
    #status { margin-bottom: 30px; color: #aaa; font-size: 16px; font-weight: bold;}
    
    /* Container para centralizar o joystick */
    #joystick-container { position: relative; width: 300px; height: 300px; display: flex; align-items: center; justify-content: center; }
    
    /* Canvas estilizado como NippleJS */
    canvas { 
      background-color: rgba(255, 255, 255, 0.05); /* Fundo semi-transparente */
      border-radius: 50%; 
      border: 2px solid rgba(255, 255, 255, 0.1); 
      touch-action: none; 
      box-shadow: inset 0 0 15px rgba(0,0,0,0.5); 
    }
  </style>
</head>
<body>
  <h2>STEM Robot</h2>
  <div id="status">Conectando ao Carrinho...</div>
  <div id="joystick-container">
    <canvas id="joystick" width="300" height="300"></canvas>
  </div>

  <script>
    var canvas = document.getElementById('joystick');
    var ctx = canvas.getContext('2d');
    var width = canvas.width;
    var height = canvas.height;
    var radius = width / 2;
    // Tamanhos baseados no visual do NippleJS
    var outerRadius = radius * 0.8; 
    var innerRadius = 40; 
    var max_dist = outerRadius;
    var dragging = false;
    var ws;
    var sendIntervalTimer = null;
    var lastSentForca = -1;
    var lastSentAngulo = -1;

    // Conexão WebSocket direta para a ESP-01
    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81/');
      ws.onopen = function() { 
        document.getElementById('status').innerText = "Conectado Direct AP"; 
        document.getElementById('status').style.color = "#4CAF50"; 
      };
      ws.onclose = function() { 
        document.getElementById('status').innerText = "Desconectado. Reconectando..."; 
        document.getElementById('status').style.color = "#f44336"; 
        setTimeout(initWebSocket, 2000); 
      };
    }
    initWebSocket();

    // Desenha o visual do joystick (parecido com NippleJS)
    function draw(x, y) {
      ctx.clearRect(0, 0, width, height);
      
      // Anel Externo (Estilo estático do NippleJS)
      ctx.beginPath();
      ctx.arc(radius, radius, outerRadius, 0, Math.PI * 2);
      ctx.lineWidth = 4;
      ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)';
      ctx.stroke();
      
      // Pino Interno Móvel (Estilo dinâmico do NippleJS)
      ctx.beginPath();
      ctx.arc(x, y, innerRadius, 0, Math.PI * 2);
      ctx.fillStyle = 'rgba(33, 150, 243, 0.5)'; // Azul semi-transparente
      ctx.fill();
      ctx.lineWidth = 2;
      ctx.strokeStyle = 'rgba(33, 150, 243, 0.8)';
      ctx.stroke();
    }
    
    // Inicia centralizado
    draw(radius, radius); 

    // Função que envia o JSON no formato EXATO esperado pelo Arduino
    function transmitData(forca, angulo) {
      if (ws && ws.readyState === WebSocket.OPEN) {
        // Formato: {"from": String(velocidade), "state": String(angulo)}
        var msg = JSON.stringify({ "from": String(forca), "state": String(angulo) });
        ws.send(msg);
        lastSentForca = forca;
        lastSentAngulo = angulo;
      }
    }

    // Gerencia o envio de dados (instantâneo + contínuo)
    function sendData(forca, angulo, forceSend = false) {
      // Se for parada, cancela o timer e envia imediatamente
      if (forceSend && forca === 0 && angulo === 0) {
        if (sendIntervalTimer) clearInterval(sendIntervalTimer);
        sendIntervalTimer = null;
        transmitData(0, 0);
        return;
      }

      // Evita envios redundantes se nada mudou
      if (forca === lastSentForca && angulo === lastSentAngulo && !sendIntervalTimer) return;

      // Se moveu, envia imediatamente o primeiro pacote
      if (!sendIntervalTimer) {
        transmitData(forca, angulo);
        // E cria um timer para continuar enviando enquanto segurar (~10 vezes por segundo)
        // para manter o carrinho andando (funciona como heartbeat/comando contínuo)
        sendIntervalTimer = setInterval(function() {
          transmitData(lastSentForca, lastSentAngulo);
        }, 100); 
      } else {
        // Atualiza as variáveis que o timer está usando
        lastSentForca = forca;
        lastSentAngulo = angulo;
      }
    }

    function updateJoystick(clientX, clientY) {
      var rect = canvas.getBoundingClientRect();
      var cx = clientX - rect.left;
      var cy = clientY - rect.top;
      var dx = cx - radius;
      var dy = cy - radius;
      var dist = Math.sqrt(dx*dx + dy*dy);
      
      if (dist > max_dist) {
        var angle = Math.atan2(dy, dx);
        cx = radius + max_dist * Math.cos(angle);
        cy = radius + max_dist * Math.sin(angle);
        dist = max_dist;
      }
      
      // Atualiza visual
      draw(cx, cy);
      
      // Cálculo de Força (0-100)
      var forca = Math.round((dist / max_dist) * 100);
      
      // Cálculo de Ângulo (0-360) com CORREÇÃO de Eixo Y (Frente/Trás)
      // Originalmente dy é positivo para baixo. Invertemos para Math.atan2(-dy, dx)
      // para que Frente (dedo para cima) seja 90 graus.
      var correctedDy = -dy; 
      var angleRad = Math.atan2(correctedDy, dx);
      var anguloGraus = Math.round(angleRad * (180 / Math.PI));
      
      // Converte para intervalo 0-360 (Padrão: 0=Direita, 90=Frente, 180=Esquerda, 270=Trás)
      if (anguloGraus < 0) anguloGraus += 360;
      
      sendData(forca, anguloGraus);
    }

    // Ao soltar o joystick: volta ao centro e envia PARADA (0,0) instantaneamente
    function resetJoystick() {
      draw(radius, radius); // Visual ao centro
      sendData(0, 0, true); // Envio forçado de parada
    }

    // Eventos Touch (Telemóvel)
    canvas.addEventListener('touchstart', function(e){ dragging = true; updateJoystick(e.touches[0].clientX, e.touches[0].clientY); e.preventDefault(); }, {passive: false});
    canvas.addEventListener('touchmove', function(e){ if(dragging) updateJoystick(e.touches[0].clientX, e.touches[0].clientY); e.preventDefault(); }, {passive: false});
    canvas.addEventListener('touchend', function(e){ dragging = false; resetJoystick(); e.preventDefault(); }, {passive: false});
    
    // Eventos Mouse (PC)
    canvas.addEventListener('mousedown', function(e){ dragging = true; updateJoystick(e.clientX, e.clientY); });
    window.addEventListener('mousemove', function(e){ if(dragging) updateJoystick(e.clientX, e.clientY); });
    window.addEventListener('mouseup', function(e){ if(dragging) { dragging = false; resetJoystick(); } });
  </script>
</body>
</html>
)=====";

#endif