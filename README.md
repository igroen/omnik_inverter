## Example InfluxDB usage:
To insert solar panel metrics to a local InfluxDB instance using the `influx` command run:
```
influx -database <database> -execute "insert <measurement> $(timeout 5 omnik_stats -s <serial_number> -h <host> -p <port> -i)
```
To insert metrics using the `curl` command run:
```
curl -i -XPOST http://localhost:8086/write?db=<database> --data-binary "<measurement> $(timeout 5 omnik_stats -s <serial_number> -h <host> -p <port> -i)"
```
