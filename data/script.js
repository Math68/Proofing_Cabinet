
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
  window.addEventListener('load', onLoad);
  
  function onLoad(event){
    initWebSocket();
  }

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
      document.getElementById("RunModePicture").getAttributeNode("src").value = "/Cooling.png";
    }
    else if(event.data == "Heating"){
      let element = document.getElementById("RunModePicture")
      if (element) {
        element.getAttributeNode("src").value = "/Heating.png";
      }
    } else if (event.data.startsWith("temp:")){
      let temp = event.data.replace("temp:", "");
      document.getElementById("CabinetTempID").innerHTML = temp;
    }
  }