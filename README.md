# Fixed Foveated Rendering

**Contributers**: Lukas Hinterleitner, Daniel Nepp

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

### Features
- Einfache Szene als Grundlage umm die Effekte des foveated renderings zu veranschaulichen.

- Simple Maus & Keyboard Steuerung um die Szene perspektivisch zu manipulieren und die Auswirkungen des renderings in den neuen Perspektiven betrachten zu können.

- Der Fokuspunkt (welche standardmäßig in der Bildschirmmitte liegt) kann mithilfe der Pfeiltasten umpositioniert werden um den rendering Effekt an unterschiedlichen stellen zu erreichen. (Simuliert das eye tracking!)

- Der Radius des Sichtfeldes kann mittels der [+] und [-] Tasten vergrößert und verkleinert werden. (Radius wird zwar an die Shader übergeben, aber hat im aktuellen Stand des Projektes noch keine Auswirkung)

- Zwei Shader Programme, um einerseits eine log-polar Transformation der Szenen Vertices in einen Texture Buffer zu rendern und anschließend die Pixel aus dem Textur Buffer wieder in das kartesische Koordinatensystem zu interpolieren und auf den Bildschirm zu rendern. 

## First Run
Das Projekt ist so konfiguriert, so dass nur wenige Aktionen benötigt werden, um das Programm zu laufen zu bringen.
Die Datei Fovea.sln öffnet die Solution und anschließend muss als Build Konfiguration Debug x86 ausgewählt werden, denn es existiert bereits ein Ausgabeverzeichnis namens Debug, in dem sich die benötigten DLL-Dateien befinden.

Im Anschluss kann das Projekt gebuildet und gestartet werden.

## Quellen
Der konzeptionelle und mathematische Hintergrund für dieses Projekt stammt aus folgender Veröffentlichung: <br>
<a>https://duruofei.com/papers/Meng_KernelFoveatedRendering_I3D2018.pdf</a> <br>
Implementiert wurde der algorithmus mithilfe folgender Tutorials: <br>
<a>https://learnopengl.com/Advanced-Lighting/Bloom</a> <br>
<a>https://www.lighthouse3d.com/tutorials/glsl-tutorial/texture-coordinates/</a> <br>

---

## Kern Code


Vertex Punkte polarisieren:
```
vec4 fovea_to_log_polar(vec4 v) 
{
	vec2 fv_vec = vec2( v.x-fx, v.y-fy ); // subtracting fovea

	float vp = _log( // logarithmic distance!
		base,
		pow(pow(fv_vec.x, 2) + pow(fv_vec.y, 2), 0.5) // distance
	);
			
	float va = atan( fv_vec.y, fv_vec.x  ); // polar angle
	vec3 v_pol; // vertex  radial

	v_pol.x = vp;
	v_pol.y = va;
	v_pol.z = v.z;

	return vec4( v_pol, v.w );
}
```

Fragment Pixel zurück in kartesische Koordinatensystem interpolieren:
```
vec4 fovea_to_cartesian(vec4 in_) // transform polar coordinates to cartesian coordinates
{
	float vp = in_.x; // p is x-direction in output
  	float va = in_.y; // alpha is y-direction in output
  
	float vd = pow(base, vp);
	float x = vd * cos(va) + fx; // adding fovea
  	float y = vd * sin(va) + fy;
	
  	return vec4(x, y, in_.z, in_.w);
}
```
