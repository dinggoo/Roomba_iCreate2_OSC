# Roomba_iCreate2_OSC
 
De wifi-routervan TP-LINK! Deze is al ingesteld op de netwerknaam 'songline' en er zit géén wachtwoord op.
Het ip adres van de router is 10.0.1.1
Wil je als 'admin' inloggen dan is het wachtwoord : Pr1ntsh0p ( 0 = nul ).

Start de boel als volgt op: 

1. Zet de wifi router aan en wacht een paar minuten om op te starten.

2. Maak verbinding met je laptop met het wifi netwerk 'songline' ..zonder wachtwoord.

3. Doe het rode draadje van het het ESP32 printje op de + pinheader

4. De esp32 start op en er gaat een blauw lampje uiteindelijk branden. Dan is de esp32 verbonden met de wifi-router.

5. Via de admin pagina onder het kopje 'dhcp-clients' kun je zien welk ip adres de esp32 heeft gekregen.

6. In de Max patch verander je het ip adres in de udpsend node in het adres wat je terug vond hierboven.

7. Druk in de patch op 
/full of /safe.. dan moet je daarna in staat zijn de knoppen /fwd of /bckw te drukken om de Roomba te laten rijden.


Ik heb de esp32 met het printje op de Roomba wat vast ge-taped.

Je kunt de esp32 nu via de button in config mode zetten en dan de verbinding met je accesspoint instellen.

Als je dan op je accesspoint kunt terugzien welk ip hij heeft gekregen kun je die weer invullen bij de Max patch.

Eventueel kun je even een usb kabel in de esp32 stoppen want er wordt in die debug informatie ook het verkregen ip adres geprint.

Als alles goed verbonden is dan gaat op de esp32 het blauwe ledje branden en gaat op de Roomba de led van rood -> oranje -> groen -> en uit.

 
Het rode draadje gaat op de pin met het + teken. Dan krijgt de esp32 voeding. Die zou ik loshalen als de Roomba aan het laadstation gaat. Dat is namelijk een veel hoger voltage dan de batterij spanning.

 
