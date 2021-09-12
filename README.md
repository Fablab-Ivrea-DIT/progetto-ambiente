# Progetto Ambiente

Il progetto si propone di realizzare un sistema open per la condivisione di dati geolocalizzati relativi all'inquinamento ambientale.

Questo progetto nasce con lo scopo di gettare le basi per realizzare un sistema centralizzato per il monitoraggio dello stato di inquinamento del territorio, basi che possono poi essere riproposte alle scuole della zona che possono contribuire attraverso nuovi progetti, per la realizzazione nuove centraline per il monitoraggio o per il potenziamento di quanto costruito.

# Struttura

Nella sua prima iterazione, il progetto si compone da quattro moduli essenziali: 

 - Modulo di acquisizione dei dati
 - Modulo di trasmissione dei dati
 - Server per il trattamento dei dati
 - Interfaccia web per la fruizione di tali dati

I quattro moduli sono studiati per essere sviluppati in modo del tutto indipendente, con interfacce di comunicazione preconcordate. In questo modo sara' possibile sostituire un solo modulo per volta per effettuarne eventuali miglioramenti/potenziamenti.

## Acquisizione dei dati

L'acquisizione dei dati viene effettuata attraverso un dispositivo Arduino, munito di sensori adeguati e programmato attraverso il codice di Luigi (ancora da far pervenire e da includere nel repository). I dati recuperati da questo software verranno inviati al modulo successivo tramite la relativa interfaccia, descritta nella prossima sezione

## Trasmissione dei dati

Per poter trasmettere i valori dei sensori al server si presuppone di avere a disposizione una connessione WiFi, a cui la scheda Arduino potra' collegarsi tramite un modulo wifi connesso tramite porta seriale. Verra' sviluppata una libreria apposita per l'utilizzo di questo modulo, che si proporra' di ricevere i valori dei sensori dal sistema di acquisizione per inviarli al server centrale tramite una chiamata alle API REST disponibili sul server

## Stoccaggio e trattamento dei dati

E' necessario sviluppare un server centrale per poter raccogliere tutti i dati trasmessi dai vari dispositivi e poi metterli a disposizione dai software di fruizione (in questo momento, solamente l'interfaccia web, ma in futuro i software potrebbero essere molteplici). Indicativamente, le API da sviluppare dovranno occuparsi delle seguenti funzioni:
  - API per il salvataggio dei dati (endpoint che andra' condiviso con il modulo di trasimssione dei dati) 
  - API per la geolocalizzazione a l'aggiunta di informazioni ad ogni flusso di dati 
  - API per la ricezione dei dati aggregati, utilizzata per aggiornare regolarmente l'interfaccia web
  - API per la consultazione storica dei dati

## Interfaccia web per la fruizione dei dati 

Per poter fruire i dati ci si prepone di utilizzare un'interfaccia web in grado di scaricare i dati dal server centrale e mostrarli, possibilimente abbinati ad una mappa del territorio. Questo e' il modulo che ha il maggior numero di possibili iterazioni: e' possibile partire da una semplice cartina con indicati i valori di inquinamento posizionati geograficamente nelle posizioni piu' corrette per arrivare a veri e propri strumenti di data warehouse per l'analisi andamentale dei valori registrati. Ci sono molti strumenti di analisi open collegabili a questa interfaccia, che andranno valutati in futuro.

# Implementazione 

## Acquisizione dei dati

Nella cartella <_device_> e' presente il codice da caricare sul device per l'acquisizione

## Trasmissione dei dati 

TBD - Libreria Arduino da scrivere per il modulo ESP 

## Stoccaggio e trattamento dei dati

Sotto <_server_> e' presente il progetto Python Flask che implementa i seguenti endpoints:

### Data upload

  Endpoint utilizzato dal device per inviare i dati rilevati dai propri sensori.
  La strategia di questo endpoint consiste nell'inserire direttamente in url l'identificativo del dispositivo, in modo che anche direttamente dai log delle chiamate sia possibile distinguere i vari dispositivi, mentre come parametro PUT va inserita la stringa con tutti i valori dei sensori. La stringa e' composta da tante coppie chiave:valoreAVG:valoreMIN:valoreMAX separate dal simbolo |, dove le chiavi sono le sigle dei relativi parametri (es. P25, P10), ed i tre valori indicano rispettivamente il valore medio, il minimo ed il massimo. 
  Il parametro "token" e' un codice segreto fornito in fase di registrazione del dispositivo.

  * **URL:** 
    /v0/measures/{device_id}
  * **Method:**

    `POST`

  * **Data Params (URL encoded):**

    **Required**
    `values=[alphanumeric]`
    `token=[alphanumeric]`

  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** `{ status: 'ok' }`

  * **Error Response:**

    * **Code:** 400 <br />
      **Content:** `{ status: 'fail', reason: 'Invalid values' / 'Unauth' }`
    
  * **Sample Call:**

    ```
    import requests
    r = requests.put('https://your_url_here/v0/measures?device=A001', { 'values': 'P25:1929.3:1502.2:2123.2|P10:992:901:1023', 'token': 'XXXXXX' })
    ```

### Data fetch (latest)

  Endpoint utilizzato dal web client per ottenere l'ultimo valore di tutti i sensori inviato dai vari dispositivi

  * **URL:** 
    /v0/measures/last
  * **Method:**

    `GET`

  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** 
   
    ``` 
    {
	"status": "ok",
	"devices": [
	{
		"id": "A001",
		"meta": {
			"nome": "I.I.S. G.Cena",
			"desc": "Descrizione <b>generica</b>",
			"lat": 45.467,
			"lon": 7.876
		},
		"values": {
			"pm2": 15.2,
			"pm10": 92.2
		}
	},
	...
	]
    }
    ```
    
  * **Notes:**

    Tutto l'oggetto "meta" puo' essere vuoto, se il dispositivo non e' ancora stato geolocalizzato

  * **Sample Call:**

    ```
    import requests
    r = requests.get('https://your_url_here/v0/measures/last', {})
    ```
    
### Data fetch (history)

  Endpoint utilizzato dal web client per ottenere lo storico dei valori di uno o piu' sensori/dispositivi.
  Il parametro "devices" permette di specificare l'elenco di dispositivi a cui siamo interessati, espressi come device ID separati dal simbolo |. Se assente, il sistema mostrera' tutti i device.
  Il parametro "sensors" permette di specificare l'elenco di sensori a cui siamo interessati, espressi come measure ID separati dal simbolo |. Se assente, il sistema mostrera' tutti i sensori.
  Il parametri "amount" indica il numero di valori da scaricare. Se omesso, e' pari a 1000. Questo valore e' cumulativo su tutti i dispositivi e su tutti i sensori
  I campi "from" e "to" permettono di indicare una finestra temporale in cui scaricare i dati. Se omesso, "to" e' pari a NOW() e  "from" viene ignorato, semplicemente vengono riportati abbastanza valori da esaurire il db oppure da raggiungere il limite indicato da "amount".
  Tutti i valori sono ordinati per data, in ordine decrescente
  

  * **URL:** 
    /v0/measures/all
  * **Method:**

    `GET`
    
  * **URL Params:**

    **Optional**
    `devices=[alphanumeric]`
    `sensors=[alphanumeric]`
    `amount=[decimal]`
    `from=[timestamp]`
    `to=[timestamp]`

  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** 
   
    ``` 
    {
	status: "ok",
	amount: 123,
	values: [
	  { device_id: "A001",  sensor_id: "PM25", date: 1631457975, value: 2933, value_min: 2500, value_max: 2400 },
	  { device_id: "A001",  sensor_id: "PM10", date: 1631457975, value: 1023, value_min: 1000, value_max: 1100 },
	  { device_id: "A002",  sensor_id: "PM25", date: 1631457970, value: 2500, value_min: 2400, value_max: 2600 },
	  ...
	]
    }
    ```

  * **Sample Call:**

    ```
    import requests
    r = requests.get('https://your_url_here/v0/measures/all', { amount: 100 })
    ```

### Registrazione device

  Endpoint utilizzato dal client web per registrare un nuovo dispositivo. Questo endpoint e' protetto dall'api di login, che va richiamata prima di poter fare accesso a questa chiamata
  
  * **URL:**
    /v0/devices
  * **Method:**
    
    `PUT`
    
  * **Data Params (URL encoded):**
   
    `lat=[numeric]`
    `lon=[numeric]`
    `nome=[alphanumeric]`
    
    **Optional**
    
    `desc=[alphanumeric]`
    
  * **Success Response:**

     * **Code:** 200 <br />
       **Content:** `{ status: 'ok', id: 1234, token: 'XXXXX' }`

  * **Error Response:

    * **Code:** 400 <br />
      **Content:** `{ status: 'fail', reason: 'Unauth' }`
      
  * **Sample Call:**

    ```
    import requests
    r = requests.put('https://your_url_here/v0/devices', { lat:12.2324254, lon:45.2345435, name: 'Test Device', desc': 'This is a test device with random coords' })
    ```
      
### Elenco device
  
  Endpoint utilizzabile per ottenere l'elenco completo di tutti i dispositivi registrati
  
  * **URL:**
    /v0/devices
  * **Method:**
    
    `GET`
        
  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** 
   
    ``` 
    {
	"status": "ok",
	"devices": [
	{
		"id": "A001",
		"meta": {
			"nome": "I.I.S. G.Cena",
			"desc": "Descrizione <b>generica</b>",
			"lat": 45.467,
			"lon": 7.876
		}
	},
	...
	]
    }
    ```
    
  * **Sample Call:**

    ```
    import requests
    r = requests.get('https://your_url_here/v0/devices', {})
    ```
      
### Aggiornamento metadati

  Endpoint utilizzato dal client web per associate metadati ad un dispositivo.
  Tutti i parametri sono opzionali, in modo che questa API possa essere richiamata per aggiornare anche solo un valore. Anche questa API e' protetta dal login

  * **URL:** 
    /v0/devices/{device_id}
  * **Method:**

    `PUT`

  * **Data Params (URL encoded):**

    **Optional**
    `nome=[alphanumeric]`
    `desc=[alphanumeric]`
    `lat=[numeric]`
    `lon=[numeric]`

  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** `{ status: 'ok' }`

  * **Error Response:**

    * **Code:** 400 <br />
      **Content:** `{ status: 'fail', reason: 'Invalid values'/'Unauth' }`
    
  * **Sample Call:**

    ```
    import requests
    r = requests.put('https://your_url_here/v0/device/A001', { nome='I.I.S. G.Cena', desc='nuova descrizione' })
    ```

### Rimozione dispositivo

  Endoint utilizzabile dal client web per eliminare un dispositivo.
  
  * **URL:** 
    /v0/devices/{device_id}
  * **Method:**

    `DELETE`


  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** `{ status: 'ok' }`

  * **Error Response:**

    * **Code:** 400 <br />
      **Content:** `{ status: 'fail', reason: 'Unauth' }`
    
  * **Sample Call:**

    ```
    import requests
    r = requests.delete('https://your_url_here/v0/device/A001', { })
    
### Elenco parametri

  Endoint utilizzabile dal client web per ottenere l'elenco dei parametri registrati ed i relativi codici
  
  * **URL:** 
    /v0/parameters
  * **Method:**

    `GET`


  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** 
   
    ``` 
    {
	"status": "ok",
	"parameters": [
	    { id: "P10", desc: 'Valore del PM-10' },
	    { id: "P25", desc: 'Valore del PM-2.5' },
	    ...
	]
    }
    ```
    
  * **Sample Call:**

    ```
    import requests
    r = requests.get('https://your_url_here/v0/parameters', { })

### Registrazione parametri

  Endoint utilizzabile dal client web per aggiungere un nuovo parametro/sensore al sistema
  
  * **URL:** 
    /v0/parameters
  * **Method:**

    `POST`

  * **Data Params (URL encoded):**

    `id=[alphanumeric]`
    `desc=[alphanumeric]`
    
  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** 
   
    ``` 
    {
	status: "ok",
	id: "XXX"
    }
    ```
    
  * **Sample Call:**

    ```
    import requests
    r = requests.put('https://your_url_here/v0/parameters', { id: 'PM25', desc: 'Valore PM 2.5' })

### Eliminazione parametro

  Endoint utilizzabile dal client web per eliminare un  parametro/sensore dal sistema
  
  * **URL:** 
    /v0/parameters/{parameter_ID}
  * **Method:**

    `DELETE`

  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** `{ status: 'ok' }`

  * **Error Response:**

    * **Code:** 400 <br />
      **Content:** `{ status: 'fail', reason: 'Unauth' }`
    
  * **Sample Call:**

    ```
    import requests
    r = requests.delete('https://your_url_here/v0/parameters/PM25', {} )

### Ping

  Endpoint utilizzato dai dispositivi per testare la connessione

  * **URL:** 
    /v0/ping
  * **Method:**

    `GET`

  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** 
      `{ status: 'ok', message: 'pong' }`
      
      
### Login

  Endoint utilizzabile dal client web per autenticarsi ed essere in grado di utilizzare le API piu' delicate. Una chiamata corretta a questo endpoint aggiunge una variabile di sessione che autentica tutte le chiamate successive avvenute con lo stesso cookie
  
  * **URL:** 
    /v0/login
  * **Method:**

    `POST`

  * **Data Params (URL encoded):**

    `user=[alphanumeric]`
    `pass=[alphanumeric]`
    
  * **Success Response:**

    * **Code:** 200 <br />
      **Content:** `{ status: 'ok' }`

  * **Error Response:**

    * **Code:** 400 <br />
      **Content:** `{ status: 'fail', reason: 'Unauth' }`
    
  * **Sample Call:**

    ```
    import requests
    r = requests.post('https://your_url_here/v0/login', { user: 'email@host.dom', pass: 'xxxxxx' })


Tutti i dati sono memorizzati su di un db MySQL, la cui struttura e' ancora in via di definizione.

## Fruizione dei dati (WEB)

Sotto <_web_> e' presente il web client statico in grado di comunicare con il server e di mostrare i dati geolocalizzati tramite OpenStreetMap

# Nice to have per il futuro

- Meccanismo di sicurezza  per autenticare e validare i dati ricevuti dai vari dispositivi
- Adozione di shield LoRa per trasmettere i valori dei sensori a distanza, riducendo la necessita' di connettivita' internet tramite reti wifi
- Aggiunta di funzioni di storage per permettere la trasmissione dei valori su reti intermittenti senza perdita di informazioni storiche
- Potenziamento del server di stoccaggio tramite motori di database scalabili (e.g. sistemi open NoSQL)
- Aggiunta di un'interfaccia web protetta da autenticazione per l'aggiunta di metadati ai vari flussi di informazioni (e.g. aggiungere i dati della scuola che sta trasmettendo i valori dell'inquinamento, geolocalizzarla etc. direttamente da web)
