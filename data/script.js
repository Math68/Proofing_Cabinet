function powerOff(){

    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "off", true);
    xhttp.send();
}

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
