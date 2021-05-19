# SmartCell-xDot v2

Firmware para nodo controlador de luminaria.

Implementado con modulo xDot.

Version actual: 2.7.0

Version de lora MAC: 1.0.3

Librerias de terceros:
- MBED 5.13.4
- lib xDot 3.2.5

---
## Funciones

- 5 modos de operacion:
  - Manual
  - Fotocelda
  - Curvas
  - On Off por tiempo
  - Demostracion
- Reporte periodico de:
  - Dimming
  - Potencia
  - Energia (desde el ultimo envio)
  - Modo actual
- Sincronizacion periodica de hora para evitar corrimiento.
- 3 curvas de dimming estaticas.
- 1 curva de dimming definida por el usuario.
- Recuperacion de configuracion ante cortes de energia.
- 2 Leds indicadores de estado y errores.

---

## Modos de operacion

### Modo manual
Nivel de dimming constante.

- 0% es apagado
- 100% es la luminaria encendida al maximo

### Encendido por fotocelda.
La luminaria enciende cuando se detecta poca luz en el ambiente.

**Este es el modo seguro**

### Seguimiento de curva.
El nivel de dimming varia con la hora. Se permiten niveles rangos de 1 hora y niveles de dimming en multiplos de 10.

*Nota: Si la hora no logra sincronizarse, el nodo va al modo seguro.*

Existen 3 curvas estaticas:
| Hora    	| Curva 1 	| Curva 2 	| Curva 3 	|
|:-------:	|---------	|---------	|---------	|
| *Indice* 	| 0       	| 1       	| 2       	|
|          	|         	|         	|         	|
| 8 - 19    | 0       	| 0       	| 0       	|
| 19 - 20 	| 80      	| 80      	| 80      	|
| 20 - 21 	| 100     	| 100     	| 100     	|
| 21 - 22 	| 100     	| 100     	| 100     	|
| 22 - 23 	| 100     	| 80      	| 80      	|
| 23 - 0  	| 80      	| 50      	| 80      	|
| 0 - 1   	| 60      	| 30      	| 50      	|
| 1 - 2   	| 40      	| 30      	| 50      	|
| 2 - 3   	| 40      	| 30      	| 50      	|
| 3 - 4   	| 40      	| 30      	| 50      	|
| 4 - 5   	| 40      	| 30      	| 50      	|
| 5 - 6   	| 40      	| 30      	| 50      	|
| 6 - 7   	| 60      	| 50      	| 80      	|
| 7 - 8   	| 100     	| 100     	| 100     	|
| 8 - 19   	| 0       	| 0       	| 0       	|

Existe una curva que puede ser definida remotamente por el usuario. Esta curva tiene indice 3.

### On Off por tiempo
Se define una hora de encendido y una hora de apagado. Durante el periodo en que deba estar encendido, la luminaria permanecera en 100% de dimming.

*Nota: Si la hora no logra sincronizarse, el nodo va al modo seguro.*

### Demostracion

El modo demostracion permite mostrar distintos niveles de dimming en poco tiempo (8 minutos en total).

Cada 1 minuto reduce el dimming en escalones de 20% hasta llegar al 20%. Luego comienza a subir en saltos de 20% hasta llegar al 100% y repite.


---
## Comandos uplink y downlink

[Comandos.xlsx](./Docs/Comandos.xlsx)

## Referencia de los leds

[Secuencias de leds](./Docs/secuencias%20led.xlsx)
