<?php

function twitterNonce() {
	return substr(
		rtrim(base64_encode(md5(microtime())),"=") .
		rtrim(base64_encode(md5(time())),"="),
		0, 32
	);
}


function importTweet(&$tweet, $feed=false) {
	global $db;
	static $uids = array();

	if (!is_object($tweet)) return;
	if (substr($tweet->text, 0, 1) === '@') return;
	if ($db->idExists('pudl_twitter_data', 'tweet_id', $tweet->id_str)) return;

	//Store and cache Twitter User ID information
	$uid = $tweet->user->id_str;
	if (!isset($uids[$uid])) {
		$uids[$uid] = $tweet->user->screen_name;
		$db->insertIgnore(
			'pudl_twitter_account',
			array(
				'twitter_id'	=> $uid,
				'twitter_name'	=> $uids[$uid],
			), true
		);
	}

	//Store raw Tweet data as JSON encoded string
	$db->insertIgnore(
		'pudl_twitter_data',
		array(
			'tweet_id'		=> $tweet->id_str,
			'twitter_id'	=> $uid,
			'tweet_time'	=> strtotime($tweet->created_at),
			'tweet_data'	=> json_encode($tweet),
		), true
	);

	if (is_array($feed)) foreach ($feed['parent_id'] as $key => $val) {
		$db->insert('pudl_feed', array(
			'parent_id'		=> reset($val),
			'parent_type'	=> $feed['parent_type'],
			'feed_timestamp'=> strtotime($tweet->created_at),
			'feed_source'	=> 'twitter',
			'feed_text'		=> $tweet->text,
		), true);
	}
}

function importTweets(&$tweets, $feed=false) {
	if (is_object($tweets)) {
		importTweet($tweets, $feed);
	} else if (is_array($tweets)) {
		foreach ($tweets as &$tweet) importTweet($tweet, $feed);
	} else if (is_string($tweets)) {
		$data = json_decode($tweets);
		if (empty($data)) return;
		foreach ($data as &$tweet) importTweet($tweet, $feed);
	}
}


function twitterString($uri, $method, $params) {
	$r = array();
	ksort($params);
	foreach($params as $k=>$v) $r[] = "$k=" . rawurlencode($v);
	return $method . '&' . rawurlencode($uri) . '&' . rawurlencode(implode('&', $r));
}
 

function twitterHeader($oauth) {
	$values = array();
	foreach($oauth as $k=>$v) $values[] = "$k=\"" . rawurlencode($v) . "\"";
	return 'Authorization: OAuth ' . implode(', ', $values);
}


function twitterAPI($twitter_uri, $method='GET') {
	global $site, $af; 

	$twitter_url = $twitter_uri;
	$pos = strpos($twitter_url, '?');
	if ($pos > 0) $twitter_url = substr($twitter_url, 0, $pos);

	$parts = parse_url($twitter_uri);
	if (empty($parts['query'])) $parts['query'] = '';
	parse_str($parts['query'], $params);


	// Set up the oauth Authorization array
	$oauth = array(
		'oauth_consumer_key'		=> $site['tw_app_id'],
		'oauth_nonce'				=> twitterNonce(),
		'oauth_signature_method'	=> 'HMAC-SHA1',
		'oauth_token'				=> $site['tw_user_token'],
		'oauth_timestamp'			=> $af->time(),
		'oauth_version'				=> '1.0',
	);

	echo "\n\n" . twitterString($twitter_url, $method, array_merge($oauth, $params)) . "\n\n";

	$oauth['oauth_signature'] = base64_encode(hash_hmac(
		'sha1',
		twitterString($twitter_url, $method, array_merge($oauth, $params)),
		rawurlencode($site['tw_app_secret']) . '&' . rawurlencode($site['tw_user_secret']),
		true
	));

	ksort($oauth);


	$feed = curl_init();
	if ($method === 'POST') {
		curl_setopt($feed, CURLOPT_POST, 1);
		curl_setopt($feed, CURLOPT_POSTFIELDS, $parts['query']);
	}

	curl_setopt($feed, CURLOPT_URL, $twitter_uri);

	curl_setopt_array($feed, array(
		CURLOPT_HTTPHEADER => array(twitterHeader($oauth), 'Expect:'),
		CURLOPT_HEADER => false,
		CURLOPT_RETURNTRANSFER => true,
		CURLOPT_SSL_VERIFYPEER => false,
	));

	$result = curl_exec($feed);

	curl_close($feed);

	return $result;
}
