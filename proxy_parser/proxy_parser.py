import requests
import json

def main():
	response = requests.get("https://gitlab.com/cto.endel/atack_hosts/-/raw/master/hosts.json")
	api_list = response.json()
	proxy_file_content = ""
	for api in api_list:
		api_json = None
		try:
			api_resp = requests.get(api, timeout=15)
			api_json = api_resp.json()
		except:
			print("%s UNAVAILABLE" % (api))
			continue

		if "proxy" in api_json:
			for proxy in api_json["proxy"]:
				proxy_file_content += proxy["ip"].strip() + "|" + proxy["auth"].strip() + "\n"


	with open("proxies.txt", "w") as f:
		f.write(proxy_file_content)

main()