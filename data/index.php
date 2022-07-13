<!doctype html>

<html lang="de">
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta charset="utf-8">
        <link rel ="shortcut icon" type="image/png" href ="water.png">
        <link href='https://fonts.googleapis.com/css?family=ABeeZee' rel='stylesheet'>
        <link rel="stylesheet" type="text/css" href="style.css">
        <link rel="stylesheet" type="text/css" href="button.css">
        <link rel='stylesheet' href='https://fonts.googleapis.com/css?family=DM%20Mono'>
        <title>Water Deluxe</title>
    </head>

    <body>
        <header class="header">
            <div class="flex-topbar">
            <nav style="text-align: left">
                <a href="http://esp8266.local/">
                    <img src="info.png" title="Startseite" alt="Startseite" width="30" height="30"></a>
                &nbsp;

                <a href="http://esp8266.local/infopage.php">
                <img src="data.png" title="Startseite" alt="Startseite" width="30" height="30"></a>
                &nbsp;
            
            </nav>
            <img src="water.png" alt="Unser Logo: Eine Trinkfalsche">
            </div>
        </header>
        <!-- Bereich fuer die Anzeige der Messdaten -->
        <div class="pagebody">
            <article style="font-size: 1.8em;">
                <br>
                <span>Aktuelles Gewicht:</span>        <span id="currentWeight"></span><br>
                <span>Maximalgewicht:</span>           <span id="maxWeight"></span><br>
                <span>Zuletzt getrunkene Menge:</span> <span id="lastDrankAmount"></span><br>
                <span>Zuletzt getrunken um:</span>     <span id="lastDrankDate"></span><br>
                <span>Letzte Woche getrunken:</span>   <span id="DrankDay"></span><br>
                <br>
            </article>
        </div>

        <!-- JavaScript -->
        <script type="text/javascript">
            //Maximalgewicht
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("maxWeight").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "maxWeight", true);
                valueRequest.send();
            }, 300);            //zweiter Parameter ist die Verzögerung der Anzeige 

            //Zuletzt getrunkene Menge
            setInterval(function () {
                var valueRequest = new XMLHttpRequest();
                valueRequest.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById("lastDrankAmount").innerHTML = this.responseText;
                    }
                };
                valueRequest.open("GET", "lastDrankAmount", true);
                valueRequest.send();
            }, 300);

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
            }, 300);

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
            }, 300);

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
            }, 300);
        </script>

        <footer class="flex-container-footer">
            <section>
                <!-- normally inline style should not be used -->
                <img style="width: 150px; height: auto" src="water.png" alt="logo">
            </section>
            <section style="margin-top: auto; margin-bottom: auto;">
                <p style="font-size: 40px; margin: 0px;" class="title"> Water Deluxe</p>
                <p>Alle Bilder: Freepik | Flaticon</p>  
            </section>
            <section>
                <a class="btn-fwd btn-st btn-light" href="http://esp8266.local/imprint.php"><span> Impressum </span></a><br>
                <a class="btn-fwd btn-st btn-light" href="http://esp8266.local/privacy.php"><span> Datenschutz </span></a><br>
            </section>
        </footer>
    </body>
</html>