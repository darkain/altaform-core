<?php

// 	parse out the URL
$url['uri']		= urldecode($get->server('REQUEST_URI', '', _GETVAR_BASIC));
if (substr($url['uri'],0,2) === '//') $af->redirect('/');

$url['offline']	= (int)!empty($url['offline']);
$url['referer'] = $get->server('HTTP_REFERER', '', _GETVAR_BASIC);
$url['https']	= strtolower($get->server('HTTPS'));
$url['https']	= ($url['https'] !== 'off'  &&  !empty($url['https']));
$url['domain']	= $get->server('HTTP_HOST', '', _GETVAR_BASIC);
$url['host']	= ($url['https']?'https://':'http://') . $url['domain'];
$url['af_host']	= $host;
$url['safe']	= array();
$url['id']		= 0;
$url			= array_merge($url, (array)parse_url($url['uri']));

if (empty($url['path'])) error404();

if (!isset($url['base'])) $url['base'] = '';
if (substr($url['path'], 0, 1) != '/') $url['path'] = '/' . $url['path'];
if (substr($url['path'], -1) != '/') $url['path'] .= '/';

$url['part'] = explode('/', $url['path']);
$url['url'] = '/';
foreach ($url['part'] as $key => $val) {
	$url['safe'][] = isset($db) ? $db->safe($val) : mysqli_real_escape_string($val);
	if (strlen($val) < 1) continue;
	$url['url'] .= urlencode($val) . '/';
}

$url['all'] = $url['host'] . $url['base'] . $url['url'];
$url['full'] = $url['all'];
if (!empty($url['query'])  &&  $url['query']!='jq=1') $url['all'] .= "?$url[query]";



//	load our web page!!
if (count($url['part']) > 2) {
	$count = count($url['part']) - 1;

	for ($i=1; $i<$count; $i++) {
		if (is_dir($url['part'][$i])) {
			chdir($url['part'][$i]);
			if ($count-$i == 1) {
				if (is_file('index.php')) {
					require_once('index.php');
					break;
				} else if (is_file('index.tpl')) {
					$af->renderPage('index.tpl');
					break;
				} else {
					error404();
				}
			}

		} else if ($count-$i == 1) {
			$file = $url['part'][$i];
			if (is_file("$file.php")) {
				require_once("$file.php");
				break;
			} else if (is_file("$file.tpl")) {
				$af->renderPage("$file.tpl");
				break;
			} else if (is_file('virtual.php')) {
				if (empty($url['virtual'])) {
					for ($x=$i; $x<$count; $x++) $url['virtual'][] = $url['safe'][$x];
				}
				$url['id'] = string_to_int($url['virtual'][0]);
				require_once('virtual.php');
				break;
			} else if (is_dir('virtual')) {
				if (empty($url['virtual'])) {
					for ($x=$i; $x<$count; $x++) $url['virtual'][] = $url['safe'][$x];
				}
				$url['id'] = string_to_int($url['virtual'][0]);
				chdir('virtual');
				if ($count-$i == 1) {
					if (is_file('index.php')) {
						require_once('index.php');
						break;
					} else if (is_file('index.tpl')) {
						$af->renderPage('index.tpl');
						break;
					} else {
						error404();
					}
				}
			} else {
				error404();
			}

		} else if (is_dir('virtual')) {
			if (empty($url['virtual'])) {
				for ($x=$i; $x<$count; $x++) $url['virtual'][] = $url['safe'][$x];
			}
			$url['id'] = string_to_int($url['virtual'][0]);
			chdir('virtual');

		} else if (is_file('virtual.php')) {
			if (empty($url['virtual'])) {
				for ($x=$i; $x<$count; $x++) $url['virtual'][] = $url['safe'][$x];
			}
			$url['id'] = string_to_int($url['virtual'][0]);
			require_once('virtual.php');
			break;

		} else {
			error404();
		}
	}
} else {
	if (is_file("_config/$url[af_host]/index.php")) {
		require_once("_config/$url[af_host]/index.php");
	} else if (is_file("_config/$url[af_host]/index.tpl")) {
		$af->renderPage("_config/$url[af_host]/index.tpl");
	} else {
		error404();
	}
}
