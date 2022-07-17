<!doctype html>

<html lang="de">
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta charset="utf-8">
        <link rel ="shortcut icon" type="image/png" href ="https://cdn-icons-png.flaticon.com/512/824/824239.png">
        <link href='https://fonts.googleapis.com/css?family=ABeeZee' rel='stylesheet'>
        <link rel="stylesheet" type="text/css" href="style.css">
        <link rel="stylesheet" type="text/css" href="button.css">
        <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=DM%20Mono'>
        <title>Hydration Tracker</title>
    </head>

    <body>
        <header class="header">
            <div class="flex-topbar">
            <nav style="text-align: left">
                <a href="http://esp8266.local/">
                    <img src="https://cdn-icons-png.flaticon.com/512/7423/7423011.png" title="Startseite" alt="Startseite" width="40" height="40"></a>
                &nbsp;

                <a href="http://esp8266.local/infopage.php">
                    <img src="https://cdn-icons-png.flaticon.com/512/263/263164.png" title="Infoseite" alt="Infoseite" width="40" height="40"></a>
                &nbsp;
            
            </nav>
            <img src="https://cdn-icons-png.flaticon.com/512/824/824239.png" alt="Unser Logo: Eine Trinkflasche">
            </div>
        </header>
        <!-- Bereich fuer die Anzeige der Messdaten -->
        <div class="pagebody">
            <article style="font-size: 1.8em;">
                <br>
                <span>Aktuelles Gewicht:</span>        <span id="currentWeight">0</span><br>
                <span>Maximalgewicht:</span>           <span id="maxWeight">0</span><br>
                <span>Zuletzt getrunkene Menge:</span> <span id="lastDrankAmount">0</span><br>
                <span>Zuletzt getrunken um:</span>     <span id="lastDrankDateHour">0</span> : <span id="lastDrankDateMinute">0</span> : <span id="lastDrankDateSecond">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay1">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay2">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay3">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay4">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay5">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay6">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay7">0</span><br>
                <br>
            </article>
        </div>

        <!-- JavaScript -->
        <script type="text/javascript">
            window.onload = function getWeight() {
                //Aktueller Füllstand
                var valueRequestCurrentWeight = new XMLHttpRequest();
                valueRequestCurrentWeight.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("currentWeight").innerHTML = this.responseText;
                    }
                };
                valueRequestCurrentWeight.open("GET", "currentWeight", true);
                valueRequestCurrentWeight.send();
            
                //Maximalgewicht
                var valueRequestMax = new XMLHttpRequest();
                valueRequestMax.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("maxWeight").innerHTML = this.responseText;
                    }
                };
                valueRequestMax.open("GET", "maxWeight", true);
                valueRequestMax.send();

                //Zuletzt getrunkene Menge
                var valueRequestLastDrankAmount = new XMLHttpRequest();
                valueRequestLastDrankAmount.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankAmount").innerHTML = this.responseText;
                    }
                };
                valueRequestLastDrankAmount.open("GET", "lastDrankAmount", true);
                valueRequestLastDrankAmount.send();

                //Zuletzt getrunken (Uhrzeit)
                var valueRequestLastDrankDateHour = new XMLHttpRequest();
                valueRequestLastDrankDateHour.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankDateHour").innerHTML = this.responseText;
                    }
                };
                valueRequestLastDrankDateHour.open("GET", "lastDrankDateHour", true);
                valueRequestLastDrankDateHour.send();

                var valueRequestLastDrankDateSecond = new XMLHttpRequest();
                valueRequestLastDrankDateSecond.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankDateSecond").innerHTML = this.responseText;
                    }
                };
                valueRequestLastDrankDateSecond.open("GET", "lastDrankDateSecond", true);
                valueRequestLastDrankDateSecond.send();

                //Letzte Woche getrunken (Menge)
                var valueRequestDrankDay1 = new XMLHttpRequest();
                valueRequestDrankDay1.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay1").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay1.open("GET", "DrankDay1", true);
                valueRequestDrankDay1.send();

                var valueRequestDrankDay2 = new XMLHttpRequest();
                valueRequestDrankDay2.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay2").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay2.open("GET", "DrankDay2", true);
                valueRequestDrankDay2.send();

                 var valueRequestDrankDay3 = new XMLHttpRequest();
                valueRequestDrankDay3.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay3").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay3.open("GET", "DrankDay3", true);
                valueRequestDrankDay3.send();

                 var valueRequestDrankDay4 = new XMLHttpRequest();
                valueRequestDrankDay4.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay4").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay4.open("GET", "DrankDay4", true);
                valueRequestDrankDay4.send();

                 var valueRequestDrankDay5 = new XMLHttpRequest();
                valueRequestDrankDay5.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay5").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay5.open("GET", "DrankDay5", true);
                valueRequestDrankDay5.send();

                 var valueRequestDrankDay6 = new XMLHttpRequest();
                valueRequestDrankDay6.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay6").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay6.open("GET", "DrankDay6", true);
                valueRequestDrankDay6.send();

                 var valueRequestDrankDay7 = new XMLHttpRequest();
                valueRequestDrankDay7.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay7").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay7.open("GET", "DrankDay7", true);
                valueRequestDrankDay7.send();
            }; 

            //Aktueller Fuellstand
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("currentWeight").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "currentWeight", true);
                valueRequest.send();
            }, 60000);

            //Zuletzt getrunken Menge
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankAmount").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "lastDrankAmount", true);
                valueRequest.send();
            }, 60000);

            //Zuletzt getrunken Uhrzeit
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankDateHour").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "lastDrankDateHour", true);
                valueRequest.send();
            }, 60000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankDateMinute").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "lastDrankDateMinute", true);
                valueRequest.send();
            }, 60000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankDateSecond").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "lastDrankDateSecond", true);
                valueRequest.send();
            }, 60000);

            //Letzte Woche getrunken (Menge)
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay1").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay1", true);
                valueRequest.send();
            }, 3600000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay2").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay2", true);
                valueRequest.send();
            }, 3600000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay3").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay3", true);
                valueRequest.send();
            }, 3600000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay4").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay4", true);
                valueRequest.send();
            }, 3600000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay5").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay5", true);
                valueRequest.send();
            }, 3600000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay6").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay6", true);
                valueRequest.send();
            }, 3600000);

            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay7").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay7", true);
                valueRequest.send();
            }, 3600000);
        </script>

        <footer class="flex-container-footer">
            <section>
                <!-- normally inline style should not be used -->
                <img style="width: 150px; height: auto" src="https://cdn-icons-png.flaticon.com/512/824/824239.png" alt="logo">
            </section>
            <section style="margin-top: auto; margin-bottom: auto;">
                <p style="font-size: 40px; margin: 0px;" class="title">Hydration Tracker</p>
                <p>Alle Bilder: Freepik | Flaticon</p>  
            </section>
            <section>
                <a class="btn-fwd btn-st btn-light" href="http://esp8266.local/imprint.php"><span> Impressum </span></a><br>
                <a class="btn-fwd btn-st btn-light" href="http://esp8266.local/privacy.php"><span> Datenschutz </span></a><br>
            </section>
        </footer>
    </body>
</html>