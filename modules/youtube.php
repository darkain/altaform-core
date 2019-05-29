<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR IMPORTING YOUTUBE VIDEO THUMBNAILS
////////////////////////////////////////////////////////////////////////////////
class youtube {


	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\altaform $af, \pudl $pudl) {
		$this->af	= $af;
		$this->pudl	= $pudl;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET A YOUTUBE ID FROM A URL
	////////////////////////////////////////////////////////////////////////////
	public function id($path) {
		$parts = parse_url($path);

		if (empty($parts['query']))	return false;
		parse_str($parts['query'], $query);

		if (empty($query['v']))		return false;
		return $query['v'];
	}




	////////////////////////////////////////////////////////////////////////////
	// IMPORT YOUTUBE DATA FROM A URL
	////////////////////////////////////////////////////////////////////////////
	public function importPath($path) {
		$id = $this->id($path);

		return (!empty($id))
			? $this->import($id)
			: false;
	}




	////////////////////////////////////////////////////////////////////////////
	// IMPORT YOUTUBE DATA FROM A YOUTUBE ID
	////////////////////////////////////////////////////////////////////////////
	public function import($id) {
		if (empty($this->af->config->google['server_key'])) {
			return false;
		}

		$path = implode([
			'https://www.googleapis.com/youtube/v3/videos?id=',
			$id,
			'&part=snippet,contentDetails&key=',
			$this->af->config->google['server_key']
		]);

		$json = @json_decode(@file_get_contents($path), true);

		if (empty($json)) return false;
		if (!isset($json['items'][0]['contentDetails']['duration']))	return false;
		if (!isset($json['items'][0]['snippet']['title']))				return false;
		if (!isset($json['items'][0]['snippet']['description']))		return false;

		$this->pudl->insert('youtube', [
			'youtube_id'			=> $id,
			'youtube_length'		=> time::duration($json['items'][0]['contentDetails']['duration']),
			'youtube_title'			=> str_replace("'", '', $json['items'][0]['snippet']['title']),
			'youtube_description'	=> $json['items'][0]['snippet']['description'],
		], true);

		return $id;
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	protected	$af		= NULL;
	protected	$pudl	= NULL;

}
