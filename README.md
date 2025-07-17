# Site Monitoring Tool
Site monitoring utility tool built with C++ 

### Features
- **Site Monitor**: Requests every x(30) seconds
- **Anomaly Detector**: Evaluates last 5 minutes (20 requests)
- **SSL Verifier**: Sees if certificates are next to their expiration
- **Notifications**: Create alerts using ubuntu notifications
- **Log System**: Saves daily logs for each website

### Installing
```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev libssl-dev libjson-c-dev libnotify-dev pkg-config
```

### Libs
- **libcurl** for http(s) requests
- **json-c** for parsing json data
- **libcurl** for notifications
- **STL** for data structures and containers

 