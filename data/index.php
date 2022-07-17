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
                <span>Zuletzt getrunken um:</span>     <span id="lastDrankDate">0</span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay">0</span><br>
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
                var valueRequestLastDrankDate = new XMLHttpRequest();
                valueRequestLastDrankDate.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankDate").innerHTML = this.responseText;
                    }
                };
                valueRequestLastDrankDate.open("GET", "lastDrankDate", true);
                valueRequestLastDrankDate.send();

                //Letzte Woche getrunken (Menge)
                var valueRequestDrankDay = new XMLHttpRequest();
                valueRequestDrankDay.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay").innerHTML = this.responseText;
                    }
                };
                valueRequestDrankDay.open("GET", "DrankDay", true);
                valueRequestDrankDay.send();
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
                        document.getElementById("lastDrankDate").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "lastDrankDate", true);
                valueRequest.send();
            }, 60000);

            //Letzte Woche getrunken (Menge)
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("DrankDay").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "DrankDay", true);
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