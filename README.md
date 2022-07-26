# Logger

Un simple système de logger dans plusieurs langages. Chaque version utilise le même concept.

Avec l'aide de [RominosTHD](https://github.com/RomainTHD) pour la version originale en Java, et l'aide de [ArthurBajt](https://github.com/ArthurBajt) pour la version Python.

## Tests

Les tests sont les mêmes pour chaque version, ils sont effectués sur ubuntu, windows et macos.

[![Java test](https://github.com/Gashmob/Logger/actions/workflows/java.yml/badge.svg)](https://github.com/Gashmob/Logger/actions/workflows/java.yml)
[![Kotlin test](https://github.com/Gashmob/Logger/actions/workflows/kotlin.yml/badge.svg)](https://github.com/Gashmob/Logger/actions/workflows/kotlin.yml)

|         | Ubuntu           | Windows          | MacOS            |
|:--------|:----------------:|:----------------:|:----------------:|
| Java    |:heavy_check_mark:|:heavy_check_mark:|:heavy_check_mark:|
| Kotlin  |:heavy_check_mark:|:x:               |:heavy_check_mark:|
| Python  |:grey_question:   |:grey_question:   |:grey_question:   |
| C++     |:grey_question:   |:grey_question:   |:grey_question:   |
| C       |:grey_question:   |:grey_question:   |:grey_question:   |
| Go      |:grey_question:   |:grey_question:   |:grey_question:   |

**Notes :**

Le test Windows pour Kotlin ne fonctionne pas : `kotlinc` n'arrive pas à compiler les sources dans le script batch (si vous avez une idée de comment fixer ça, n'hésitez pas à proposer une pull request, ou répondre à l'issue [#2](https://github.com/Gashmob/Logger/issues/2))

## Utilisation

### Java & Kotlin

Une archive jar (`Logger_Java.jar` & `Logger_Kotlin.jar`) est à disposition et utilisable en tant que librairie externe.

### C & C++ & Python & Go

Il suffit de récupérer le dossier `logger` dans les dossiers `C`, `C++`, `Python` et `Go`.
