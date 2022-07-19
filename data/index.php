<!doctype html>

<html lang="de">
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta charset="utf-8">
        <link rel ="shortcut icon" type="image/png" href ="https://cdn-icons-png.flaticon.com/512/824/824239.png">
        <link href='https://fonts.googleapis.com/css?family=ABeeZee' rel='stylesheet'>
        <link rel="stylesheet" type="text/css" href="style.css">
        <link rel="stylesheet" type="text/css" href="button.css">
        <link rel="stylesheet" type="text/css" href="index.css">
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
            <noscript> <h3 class="subtitle"> Ohne Javascript kann diese Seite nicht verwendet werden. Die Daten welche sie sehen sind nicht aussagekräftig. Wenn sie diese Webseite nutzen wollen aktivieren sie Javascript. </h3></noscript>
            <div class="flex-container">
                <section class="box">
                    <h3 class="subtitle"> Aktuelles Gewicht </h3>
                    <div class="flex-inbox">
                        <div>
                            <p class="subsubtitle"> Das zuletzt gemessene Gewicht der Flasche in Gramm </p>
                            <p id="currentWeight"> Aktuelles Gewicht:  0 </p>
                        </div>
                    </div>
                </section>
                <section class="box">
                    <h3 class="subtitle"> Maximalgewicht </h3>
                    <div class="flex-inbox">
                        <div>
                            <p class="subsubtitle"> Das maximale Gewicht der Flasche in Gramm </p>
                            <p id="maxWeight"> Maximales Gewicht:  0 </p>
                        </div>
                    </div>
                </section>
                <section class="box">
                    <h3 class="subtitle"> Zuletzt getrunken </h3>
                    <div class="flex-inbox">
                        <div>
                            <p class="subsubtitle"> Die zuletzt getrunkene Menge in Gramm </p>
                            <p id="lastDrankAmount"> Zuletzt getrunkene Menge:  0 </p>
                        </div>
                    </div>
                </section>
                <section class="box">
                    <h3 class="subtitle"> Zuletzt getrunken </h3>
                    <div class="flex-inbox">
                        <div>
                            <p class="subsubtitle"> Die Uhrzeit um welche zuletzt getrunken wurde </p>
                            <p id="lastDrank">  00 : 00 : 00 </p>
                        </div>
                    </div>
                </section>
                <section class="box">
                    <h3 class="subtitle"> Getrunken in der letzten Woche </h3>
                    <div class="flex-inbox">
                        <div>
                            <p class="subsubtitle"> Die in der letzten Woche getrunkene Menge in Gramm </p>
                            <p id="DrankLastWeek"> Letzte Woche getrunken:  0 </p>
                        </div>
                    </div>
                </section>
            </div>
        </div>

        <script type="text/javascript">
            function getValue(varName) {
                var valueRequestCurrentWeight = new XMLHttpRequest();
                valueRequestCurrentWeight.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        document.getElementById(varName).innerHTML = this.responseText;
                    }
                };
                valueRequestCurrentWeight.open("GET", varName, true);
                valueRequestCurrentWeight.send();
            }

            function update() {
                getValue("currentWeight");
                getValue("maxWeight");
                getValue("lastDrankAmount")
                getValue("lastDrank")
                getValue("DrankLastWeek")
            }

            window.onload = update();
            setInterval(function () {
                update();
            }, 86400000);
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