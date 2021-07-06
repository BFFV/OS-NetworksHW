# T3 :computer: :globe_with_meridians:

## Simulación DNS

### 1)

La IP de destino (3.3.0.2) ocupa 32 bits, siguiendo el protocolo IPv4.

### 2)

La dirección IP de origen en el router central es 192.168.1.X (dinámica), correspondiente a la IP del computador desde el que se originó el paquete.

### 3)

La IP de origen es 3.3.0.2, correspondiente a la IP del servidor DNS que envió el paquete de respuesta de vuelta hacia el origen.

### 4)

### Entrada

- Capa 1 (Física): Entra el paquete a través del cable ethernet.
- Capa 2 (Enlace): Se detecta que la dirección MAC de destino es la misma del puerto receptor, y por lo tanto se alcanzó el destino del enlace. Se procede a desencapsular el paquete de la capa de enlace.
- Capa 3 (Red): Se detecta que la dirección IP de destino es la misma del dispositivo receptor, y por lo tanto se alcanzó el destino de la red. Se procede a desencapsular el paquete de la capa de red.
- Capas 4/5/6/7 (Alto Nivel): Se detecta un paquete de tipo ICMP, por lo que se delega al proceso correspondiente.

### Salida

- Capas 4/5/6/7 (Alto nivel): Como el ID del paquete es 8 (Echo Request), se envía de vuelta un paquete ICMP con ID 0 (Echo Reply).
- Capa 3 (Red): Se invierten las direcciones IP de origen y destino para poder enviar el paquete de respuesta a través de la red.
- Capa 2 (Enlace): Se invierten las direcciones MAC de origen y destino para poder enviar el paquete de vuelta a través del router gateway.
- Capa 1 (Física): Sale el paquete a través del cable ethernet.

### 5)
### Bonus

El protocolo Ethernet busca transmitir la información a tasas altas de bits dentro de redes locales, por lo que encapsula directamente las IPs y no agrega más headers al paquete. Por otro lado, el protocolo HDLC se usa para transmitir los datos bit por bit a través de cables seriales en redes de mayor alcance, agregando más información al paquete. Por lo tanto, en el router que actúa como punto de acceso a la red local Casa Alumno se realiza la conversión entre ambos protocolos.

## Simulación Ruzpedia

### 1)

El HTTP Request del paquete HTTP tiene un tamaño de 32 bytes.

### 2)

- **DNS (Domain Name System):** Se utiliza para consultar al servidor DNS por la IP asociada al nombre del dominio web al que se desea acceder. Los paquetes deben contener el nombre del dominio (en el caso del cliente DNS) o la IP asociada al dominio (en el caso del servidor DNS).
- **HTTP (Hypertext Transfer Protocol):** Es el protocolo por excelencia para la transferencia de información en la web (en archivos XHML, HTML, etc...). Un mensaje en este protocolo está formado en primer lugar por una línea que indica el tipo de request/response (en caso de que sea cliente/servidor), seguida del __header__, que contiene metadatos de la consulta. Luego tiene el __body__, cuya presencia es opcional y contiene los datos crudos que se desean enviar (archivos, texto plano, etc...).
- **STP (Spanning Tree Protocol):** Se encarga de gestionar la presencia de bucles en las conexiones de la red, activando/desactivando enlaces cuando sea necesario para garantizar que no aparezca ningún loop infinito. Los routers comparten la información de sus conexiones directas y aplican algoritmos de grafos para tomar estas decisiones.

### 3)

-  **Parte 1: DNS**: El paquete DNS se origina en el laptop de Ruzpedia (172.67.7.3) y viaja de forma inalámbrica hacia su router local (172.67.7.1). Luego, el paquete pasa por el router central y es redirigido hacia el router de la red DNS (3.3.0.1), el que a su vez lo releva hacia el servidor DNS (3.3.0.2). Tras esto, el servidor DNS envía la respuesta en otro paquete DNS, el que sigue el camino inverso hasta volver al laptop de Ruzpedia. A continuación se avanza al paso de **HTTP**.

- **Parte 2: HTTP**: Desde el laptop de Ruzpedia (172.67.7.3), se envía una request HTTP hasta el router local (172.67.7.1), el que lo envía directamente hacia el servidor que **hostea** a Ruzpedia (172.67.7.2). A continuación, el servidor envía la response HTTP a través del camino inverso hasta llegar al laptop de Ruzpedia. A continuación se avanza a la etapa de **STP**.

- **Parte 3: STP**: El router de la subred DNS (3.3.0.1) genera un paquete STP y se lo manda tanto al servidor DNS (3.3.0.2) como al router central. Luego, el router de la red casa de Alumno realiza esto mismo, pero con el PC del alumno (192.168.1.X) y el router central. Luego el router de Ruzpedia (172.67.7.1) realiza nuevamente lo mismo, pero con su servidor de Ruzpedia (172.67.7.2) y el router central. Finalmente, se repiten estos pasos en todas las subredes de forma indefinida para ejecutar el algoritmo que se encarga de detectar bucles, enviando los paquetes STP a todos los dispositivos que se encuentran conectados directamente por cables.
