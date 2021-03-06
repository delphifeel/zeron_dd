import requests
import json

# MAIN_URL = "https://gitlab.com/cto.endel/atack_hosts/-/raw/master/hosts.json"
MAIN_URL = "https://hutin-puy.nadom.app/hosts.json"

def main():
	response = requests.get(MAIN_URL)
	api_list = response.json()
	proxy_file_content = ""
	for api in api_list:
		api_json = None
		target_site = "https://sb.ru"

		try:
			api_resp = requests.get(api, timeout=15)
			api_json = api_resp.json()
		except:
			print("%s UNAVAILABLE" % (api))
			continue

		if "proxy" not in api_json:
			continue

		if "site" in api_json:
			target_site = api_json["site"]["url"]

		for proxy in api_json["proxy"]:
			proxy_file_content += proxy["ip"].strip() + "|"
			if "auth" in proxy:
				proxy_file_content += proxy["auth"].strip()
			else:
				proxy_file_content += " "
			proxy_file_content += "|" + target_site + "\n"

	with open("proxies.txt", "w") as f:
		f.write(proxy_file_content)

main()