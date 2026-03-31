#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <title>Controle Carrinho</title>
    <style>
      body { text-align: center; font-family: 'Rubik', 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-image: linear-gradient(rgb(1, 77, 143, 0.15), rgb(104, 171, 48, 0.15)); color: #014d8f; margin: 0; overflow: hidden; height: 100vh; display: flex; flex-direction: column; align-items: center; justify-content: center;}
      h2 { margin: 0 0 10px 0; font-size: 28px; color: #014d8f; text-shadow: 0 1px 2px rgba(0,0,0,0.1); }
      #status { margin-bottom: 30px; color: #555; font-size: 16px; font-weight: bold;}
      #joystick-container { position: relative; width: 300px; height: 300px; display: flex; align-items: center; justify-content: center; }
      canvas { background-color: rgba(0, 0, 0, 0.05); border-radius: 50%; border: 2px solid rgba(1, 77, 143, 0.2); touch-action: none; box-shadow: inset 0 0 15px rgba(0,0,0,0.1); }
    </style>
  </head>
  <body>
    <h2>Controle Carrinho STEM Criar</h2>
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
      var outerRadius = radius * 0.8; 
      var innerRadius = 40; 
      var max_dist = outerRadius;
      var dragging = false;
      var ws;
      var sendIntervalTimer = null;
      var lastSentForca = -1;
      var lastSentAngulo = -1;

      function initWebSocket() {
        ws = new WebSocket('ws://' + window.location.hostname + ':81/');
        ws.onopen = function() { document.getElementById('status').innerText = "Conectado!"; document.getElementById('status').style.color = "#4CAF50"; };
        ws.onclose = function() { document.getElementById('status').innerText = "Desconectado. Reconectando..."; document.getElementById('status').style.color = "#f44336"; setTimeout(initWebSocket, 2000); };
      }
      initWebSocket();

      function draw(x, y) {
        ctx.clearRect(0, 0, width, height);
        ctx.beginPath(); ctx.arc(radius, radius, outerRadius, 0, Math.PI * 2); ctx.lineWidth = 4; ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)'; ctx.stroke();
        ctx.beginPath(); ctx.arc(x, y, innerRadius, 0, Math.PI * 2); ctx.fillStyle = 'rgba(33, 150, 243, 0.5)'; ctx.fill(); ctx.lineWidth = 2; ctx.strokeStyle = 'rgba(33, 150, 243, 0.8)'; ctx.stroke();
      }
      draw(radius, radius); 

      function transmitData(forca, angulo) {
        if (ws && ws.readyState === WebSocket.OPEN) {
          var msg = JSON.stringify({ "from": String(forca), "state": String(angulo) });
          ws.send(msg);
          lastSentForca = forca;
          lastSentAngulo = angulo;
        }
      }

      function sendData(forca, angulo, forceSend = false) {
        if (forceSend && forca === 0 && angulo === 0) {
          if (sendIntervalTimer) clearInterval(sendIntervalTimer);
          sendIntervalTimer = null;
          transmitData(0, 0);
          return;
        }
        if (forca === lastSentForca && angulo === lastSentAngulo && !sendIntervalTimer) return;
        if (!sendIntervalTimer) {
          transmitData(forca, angulo);
          sendIntervalTimer = setInterval(function() { transmitData(lastSentForca, lastSentAngulo); }, 100); 
        } else {
          lastSentForca = forca; lastSentAngulo = angulo;
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
        draw(cx, cy);
        
        var forca = Math.round((dist / max_dist) * 100);
        var correctedDy = -dy; 
        var angleRad = Math.atan2(correctedDy, dx);
        var anguloGraus = Math.round(angleRad * (180 / Math.PI));
        if (anguloGraus < 0) anguloGraus += 360;
        
        sendData(forca, anguloGraus);
      }

      function resetJoystick() {
        draw(radius, radius); 
        sendData(0, 0, true); 
      }

      canvas.addEventListener('touchstart', function(e){ dragging = true; updateJoystick(e.touches[0].clientX, e.touches[0].clientY); e.preventDefault(); }, {passive: false});
      canvas.addEventListener('touchmove', function(e){ if(dragging) updateJoystick(e.touches[0].clientX, e.touches[0].clientY); e.preventDefault(); }, {passive: false});
      canvas.addEventListener('touchend', function(e){ dragging = false; resetJoystick(); e.preventDefault(); }, {passive: false});
      canvas.addEventListener('mousedown', function(e){ dragging = true; updateJoystick(e.clientX, e.clientY); });
      window.addEventListener('mousemove', function(e){ if(dragging) updateJoystick(e.clientX, e.clientY); });
      window.addEventListener('mouseup', function(e){ if(dragging) { dragging = false; resetJoystick(); } });
    </script>
  </body>
  </html>
)=====";

const char CONFIG_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Configuração Protótipo</title>
    <style>
      body { font-family: 'Rubik', 'Segoe UI', sans-serif; text-align: center; padding: 20px; background-image: linear-gradient(rgb(1, 77, 143, 0.15), rgb(104, 171, 48, 0.15)); color: #014d8f; }
      h2 { color: #014d8f; margin-bottom: 5px; }
      input { padding: 12px; font-size: 16px; margin: 15px 0; border: 2px solid rgba(1, 77, 143, 0.3); border-radius: 8px; width: 80%; max-width: 300px; text-transform: uppercase; }
      .btn { display: inline-block; padding: 12px 24px; margin-top: 10px; background-color: #014d8f; color: white; border: none; text-decoration: none; border-radius: 5px; font-weight: bold; cursor: pointer; }
      .obs { font-size: 0.9em; color: #d32f2f; margin-top: 25px; padding: 10px; border: 1px dashed #d32f2f; border-radius: 5px; display: inline-block; max-width: 350px;}
    </style>
  </head>
  <body>
    <h2>Configuração do Protótipo</h2>
    <p>ID Atual: <strong>{{CURRENT_ID}}</strong></p>
    
    <form action='/salvar_config' method='POST'>
      <input type='text' name='novo_id' placeholder='Novo ID (Ex: CAR002)' required maxlength='15'>
      <br><button type='submit' class='btn'>Salvar Novo ID</button>
    </form>
    
    <div class='obs'><strong>⚠️ Atenção:</strong><br>Os carrinhos não podem ter o mesmo ID! Isso causará conflito nos Controles.</div>
    <br><br><a href='/' style='color: #014d8f; text-decoration: none; font-weight: bold;'>&larr; Voltar para o Controle</a>
  </body>
  </html>
)=====";

const char SAVE_CONFIG_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <style>
      body { font-family: 'Rubik', 'Segoe UI', sans-serif; text-align: center; padding: 20px; background-image: linear-gradient(rgb(1, 77, 143, 0.15), rgb(104, 171, 48, 0.15)); color: #014d8f; }
      h2 { color: #68ab30; }
      .alerta { margin-top: 30px; padding: 20px; background-color: rgba(255, 235, 59, 0.3); color: #014d8f; border: 1px solid rgba(1, 77, 143, 0.3); border-radius: 8px; font-size: 0.95em; text-align: left; display: inline-block; max-width: 400px;}
    </style>
  </head>
  <body>
    <h2>ID alterado com sucesso!</h2>
    <p>O novo ID configurado é: <br><strong style='font-size: 1.3em;'>{{NEW_ID}}</strong></p>
    
    <div class='alerta'>
      <h3 style='margin-top: 0;'>Ação Necessária</h3>
      <p>A rede Wi-Fi atual deixará de existir.</p>
      <p>Para que o novo ID entre em vigor e a nova rede apareça, <strong>desligue o protótipo da energia e volte a ligar</strong>.</p>
    </div>
    <br><br><a href='/' style='color: #014d8f; text-decoration: none; font-weight: bold;'>&larr; Voltar para o Controle</a>
  </body>
  </html>
)=====";

#endif