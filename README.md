# Informations
Un simple x-ray conçu pour Paladium<br>
Nécessite d'être injecté avec l'option "Manual Map" afin de contourner l'anticheat actuel de Paladium.<br>
Une autre possibilité est de renommer le DLL en vlc.dll, mais cela peut être patch assez facilement.

# Comment build le projet
1. Installez un JDK pour Java8
2. Téléchargez Visual Studio, un IDE qui permet de coder en C++ et de facilement build des projets
3. Clonez le projet et ouvrez le .sln fourni dedans avec Visual Studio
4. Importez les libraries et include nécessaire (includes jni/jvmti, libraries: opengl32.lib) en allant dans "Project" -> "paladium-xray Properties" -> "C/C++" -> "General" -> "Additionnal Include Directories: C:\Program Files\Java\jdk1.8.0_202\include"
5. Ctrl+B, puis allez dans le dossier de votre projet, x64, puis Debug, et le DLL build sera là.
   
# Screenshot
![screenshot](https://github.com/Wykt/paladium-xray/blob/master/screenshot.png)

# License
[GNU GPL 3.0](https://choosealicense.com/licenses/gpl-3.0/)

# Modifications
[+] Changement du nom de la classe explicite: "xray.cpp" et "xray.hpp".
[+] Ajout de block et la possibilité de changer entre Coffres et Minerais (à améliorer).
[+] Limite de chunks lors du scan.
[-] Gui supprimé, qui sera remplacé par une interface externe dans le futur.

# DISCLAIMER
* Je ne suis pas responsable de quelconque bannissement et à l'écoute de toutes remarques constructives.
