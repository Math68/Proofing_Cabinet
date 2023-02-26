
function powerOff(){
  websocket.send("powerOff");
}

// WebSocket Web Server
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;

  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
  }

  function getRunModeState(){
    websocket.send("getRunModeState");
  }
  
  function onOpen(event) {
    console.log('Connection opened');
  }

  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }

  function onMessage(event) {
    if (event.data == "Cooling"){
      //document.getElementById("RunModePicture").getAttributeNode("src").value = "/Cooling.png";
      /* let element = document.getElementById("RunModePicture");
      if(element){
        element.getAttributeNode("src").value = "/Cooling.png";
      } */

      $('#RunModePicture').attr('src', "/Cooling.png")
    }

    else if(event.data == "Heating"){
      let element = document.getElementById("RunModePicture");
      if (element) {
        element.getAttributeNode("src").value = "/Heating.png";
      }
    } 
    
    else if (event.data.startsWith("temp:")){
      let temp = event.data.replace("temp:", "");
      //document.getElementById("CabinetTempID").innerHTML = temp;
      $('#CabinetTempID').html(temp)

    } 
    
    else if (event.data.startsWith("TresholdLow:")){
      let t = event.data.replace("TresholdLow:", "");
      $('#ActualTresholdLowValue').html(t)

    }
    
    else if (event.data.startsWith("TresholdHigh:")){
      let t = event.data.replace("TresholdHigh:", "");
      $('#ActualTresholdHighValue').html(t)
    }
  }


  $(function()
  {
    initWebSocket();
  
    $('#BTsaveTresholdLow').click(function() {
      //let v = $('#TresholdLowInput').val()
      let v = $('input[name=TresholdLowInput]').val()
      websocket.send("SaveTresholdLow:" + v);
    });

    $('#BTsaveTresholdHigh').click(function() {
      let v = $('input[name=TresholdHighInput]').val()
      websocket.send("SaveTresholdHigh:" + v);
    });


  });


