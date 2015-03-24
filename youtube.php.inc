<?php

class afyoutube {

	public static function importPath($path) {
		preg_match('~https?://(www\.)?youtube\.com/watch\?v=(#!/)?@?([^/\?"\']*)~', $path, $item);
		if (empty($item[3])) return false;
		return self::import($item[3]);
	}


	public static function import($id) {
		global $db;

		$url = 'http://gdata.youtube.com/feeds/api/videos/' . $id . '?v=2&prettyprint=true&alt=json';
		$data = file_get_contents($url);
		$json = json_decode($data, true);

		if (!isset($json['entry']['media$group']['yt$duration'])) return false;
		if (!isset($json['entry']['title']['$t'])) return false;
		if (!isset($json['entry']['media$group']['media$description']['$t'])) return false;

		$db->insert('pudl_youtube', [
			'youtube_id'			=> $id,
			'youtube_length'		=> reset($json['entry']['media$group']['yt$duration']),
			'youtube_title'			=> $json['entry']['title']['$t'],
			'youtube_description'	=> $json['entry']['media$group']['media$description']['$t'],
		], true, true);

		return $id;
	}

}
