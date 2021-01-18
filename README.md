# context
Ejemplo de uso de Boost::context

**Boost::context** es una librería de procesos ligeros (user threads, coroutines).

Este ejemplo muestra como crear estos procesos ligeros, un planificador, y un método `yield( )` que suspende el proceso actual y pasa al siguiente.

Igualmente, se muestra el código necesario para controlar posibles excepciones en los procesos.
