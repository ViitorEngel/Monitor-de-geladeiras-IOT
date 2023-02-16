# Monitor-de-geladeiras-IOT
A ideia deste projeto era monitorar a temperatura e o estado (aberta ou fechada) de uma geladeira, com a finalidade de observar a perda de temperatura quando ela é aberta ou fechada para que se possa otimizar o seu uso, à fim de torná-la mais energeticamente econômica.

Para isto, utilizamos ESP32, que foram programados em C++. Para monitorar a temperatura utilizamos os sensores DS18B20, e para monitorar o estado da porta, Reed Switchs. Uma vez que os dados eram coletados e processados, eram enviados via MQTT para um broker, no qual tínhamos um dashboard desenvolvido em Node-Red inscrito.
O projeto fora realizado por:
  - Arthur Rodrigues Da Silva
  - Diogo Da Silveira
  - Pedro Luís Ourique Dos Santos Francisco
  - Vitor Renato Engel de Barros
