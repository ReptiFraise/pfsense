<?php
/*
 * syslog.inc
 *
 * part of pfSense (https://www.pfsense.org)
 * Copyright (c) 2004-2013 BSD Perimeter
 * Copyright (c) 2013-2016 Electric Sheep Fencing
 * Copyright (c) 2014-2020 Rubicon Communications, LLC (Netgate)
 * All rights reserved.
 *
 * originally part of m0n0wall (http://m0n0.ch/wall)
 * Copyright (c) 2003-2004 Manuel Kasper <mk@neon1.net>.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

require_once("globals.inc");
require_once('config.inc');

// Read the Wireguard configuration from $config.xml and create a number of *.conf files for wg-quick to read
function wg_create_config_files() {
	global $config, $g;

	if ($config["wireguard"]["tunnel"]) {
		$wg_tunnels = $config["wireguard"]["tunnel"];
		$cfgpath = $g["wg_conf_path"];

		if (!file_exists($cfgpath)) {
			mkdir($cfgpath, 0644);
		}
	}

	$idx = 0;
	foreach ($wg_tunnels as $tunnel) {
		make_wg_conf($tunnel, $idx);
		$idx++;
	}
}

function make_wg_conf($tunnel, $idx) {
	$txt = "# This Wireguard config file has been created automatically. Do not edit!\n";

	$txt .= "# Description: {$tunnel['descr']}\n\n";

	// Process Interfaces section
	$txt .= "[Interface]\n";

	if (isset($tunnel["interface"]["privatekey"]) && strlen($tunnel["interface"]["privatekey"]) > 0 ) {
		$txt .= "PrivateKey = {$tunnel["interface"]["privatekey"]}\n";
	}

	if (isset($tunnel["interface"]["address"]) && strlen($tunnel["interface"]["address"]) > 0 ) {
		$txt .= "Address = {$tunnel["interface"]["address"]}\n";
	}

	if (isset($tunnel["interface"]["listenport"]) && strlen($tunnel["interface"]["listenport"]) > 0 ) {
		$txt .= "ListenPort = {$tunnel["interface"]["listenport"]}\n";
	}

	$txt .= "\n";

	// Process peers section
	if (isset($tunnel["peer"]) && count($tunnel["peer"]) > 0) {
		global $g;

		foreach ($tunnel["peer"] as $peer) {
			$txt .= "# Peer: {$peer['descr']}\n";
			$txt .= "[Peer]\n";

			if (isset($peer["publickey"]) && strlen($peer["publickey"]) > 0 ) {
				$txt .= "PublicKey = {$peer['publickey']}\n";
			}

			if (isset($peer["address"]) && strlen($peer["address"]) > 0 ) {
				$txt .= "Address = {$peer['address']}\n";
			}

			if (isset($peer["listenport"]) && strlen($peer["listenport"]) > 0 ) {
				$txt .= "ListenPort = {$peer['listenport']}\n";
			}

			if (isset($peer["endpoint"]) && strlen($peer["endpoint"]) > 0 ) {
				$txt .= "EndPoint = {$peer['endpoint']}\n";
			}

			if (isset($peer["allowedips"]) && strlen($peer["allowedips"]) > 0 ) {
				$txt .= "AllowedIPs = {$peer['allowedips']}\n";
			}

			$txt .= "\n";
		}
	}

	file_put_contents($g["wg_conf_path"] . "/wg_" . $idx . ".conf", $txt);
}
