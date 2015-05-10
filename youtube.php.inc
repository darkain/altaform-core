<?php

class afyoutube {

	public static function id($path) {
		$parts = parse_url($path);
		if (empty($parts['query'])) return false;
		parse_str($parts['query'], $query);
		if (empty($query['v'])) return false;
		return $query['v'];
	}


	public static function importPath($path) {
		$id = self::id($path);
		if (empty($id)) return false;
		return self::import($id);
	}


	public static function import($id) {
		global $db, $site;

		$path = 'https://www.googleapis.com/youtube/v3/videos?id=' . $id . '&part=snippet,contentDetails&key=' . $site['g_server_key'];
		$data = @file_get_contents($path);
		$json = @json_decode($data, true);

		if (empty($json)) return false;
		if (!isset($json['items'][0]['contentDetails']['duration'])) return false;
		if (!isset($json['items'][0]['snippet']['title'])) return false;
		if (!isset($json['items'][0]['snippet']['description'])) return false;

		preg_match(
			'/^(-|)?P([0-9]+Y|)?([0-9]+M|)?([0-9]+D|)?T?([0-9]+H|)?([0-9]+M|)?([0-9]+S|)?$/',
			$json['items'][0]['contentDetails']['duration'],
			$matches
		);

		$seconds  =  (int)$matches[7];
		$seconds += ((int)$matches[6]) * 60;
		$seconds += ((int)$matches[5]) * 60*60;
		$seconds += ((int)$matches[4]) * 60*60*24;
		$seconds += ((int)$matches[3]) * 60*60*24*30;
		$seconds += ((int)$matches[2]) * 60*60*24*365;

		$db->insert('pudl_youtube', [
			'youtube_id'			=> $id,
			'youtube_length'		=> $seconds,
			'youtube_title'			=> str_replace("'", '', $json['items'][0]['snippet']['title']),
			'youtube_description'	=> $json['items'][0]['snippet']['description'],
		], true, true);

		return $id;
	}

}
