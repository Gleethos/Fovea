# Fixed Foveated Rendering

## Allgmein
In diesem Projekt haben wir versucht eine einfache Foveated-Rendering Implementierung zu erzielen, indem wir Szenen Vertices in das logarithmisch polare Koordinatensystem transformieren und dann die gerenderten Pixel mittels Fragment Shader wieder in das kartesische Koordinatensystem zurückinterpolieren.
Dabei war das Ziel, dass sowohl Fokuspunkt als auch Sichtfeld Radius vom Benutzer angepasst werden können.
Traditionell geht diese Rendering Technik mit dem Eyetracking einher, da wir allerdings sowohl Zeit als auch Ressourcen für eine solche Herangehensweise im Rahmen der Lehrveranstaltung nicht zur Verfügung hatten, haben wir eine Fixed-Foveated-Rendering Implementierung gewählt.

Nicht-Ziel war die Gestaltung komplexer Inhalte wie Modelle und Grafiken. Die Szene sollte dabei nur dazu dienen den Effekt des Foveated-Rendering zu veranschaulichen.

## Requirements

### Hardware
**Grafikkarte**: 		Integrierte oder dedizierte Grafikkarte mit OpenGL 3.3 oder höher <br>
**Prozessor**: 			Alle gängigen Prozessoren von Intel oder AMD reichen vollkommen <br>
**Speicherplatz**: 		mindestens 20 MB freier Speicherplatz für die Applikation inkl. Build Dateien <br>

### Software
**Betriebssystem**: 	Windows 10 <br>
**Libraries**: 			Sämtliche Libraries um OpenGL sind bereits in einem Dependencies Ordner enthalten, daher sind keine weiteren Bibliotheken notwendig <br>

## First Run
Das Projekt ist so konfiguriert, so dass nur wenige Aktionen benötigt werden, um das Programm zu laufen zu bringen.
Die Datei Fovea.sln öffnet die Solution und anschließend muss als Build Konfiguration Debug x86 ausgewählt werden, denn es existiert bereits ein Ausgabeverzeichnis namens Debug, in dem sich die benötigten DLL-Dateien befinden.

Im Anschluss kann das Projekt gebuildet und gestartet werden.