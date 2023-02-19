
function powerOff(){
  websocket.send("powerOff");
}

/*
// Get Temperature all 2s
setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("CabinetTempID").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "readCabinetTempID", true);
    xhttp.send();
},2000);
*/

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
    } else if (event.data.startsWith("temp:")){
      let temp = event.data.replace("temp:", "");
      //document.getElementById("CabinetTempID").innerHTML = temp;

      $('#CabinetTempID').html(temp)
    } else if (event.data.startsWith("TresholdLow:")){
      let t = event.data.replace("TresholdLow:", "");
      $('#ActualTresholdLowValue').html(t)
    }
  }
/*
  function ButtonSaveTresholdLow(){
    document.getElementsById('BTsaveTresholdLow').addEventListener('click',sendToServerTresholdLow);
  }

  function ButtonSaveTresholdHigh(){
    const element = document.getElementsById('BTsaveTresholdHigh');
    element.addEventListener('click',sendToServerTresholdHigh);
  }

  function sendToServerTresholdLow(){
    websocket.send("SaveTresholdLow:" + ThresholLowInput);
  }

  function sendToServerTresholdHigh(){
    websocket.send("SaveTresholdHigh:" + ThresholHighInput);
  }
*/

  $(function() {
    initWebSocket()
  
    $('#BTsaveTresholdLow').click(function() {
      //let v = $('#TresholdLowInput').val()
      let v = $('input[name=TresholdLowInput]').val()
      websocket.send("SaveTresholdLow:" + v);
    })
  })