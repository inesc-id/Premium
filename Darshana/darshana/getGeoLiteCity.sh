# download GeoLiteCity zip file
curl "http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz" -o GeoLiteCity.dat.gz
# extract the GeoLiteCity database
gzip -d GeoLiteCity.dat.gz