<?php

class afnode {

	static function push($message, $who=false) {
		global $af, $afurl, $site, $user;

		if (empty($afurl->push)) return;

		if ($who === false) $who = $user;
		if (is_array($who)) $who = $who['user_id'];
		if (empty($who)) return;

		$message = json_encode($message);

		$url = $afurl->push . '?' . http_build_query([
			'time'		=> $af->time(),
			'auth'		=> md5($af->time() . $message . $site['af_secret']),
			'user'		=> (int) $who,
			'message'	=> $message,
		]);

		if (defined('HHVM_VERSION')) {
			$shutdown = 'register_postsend_function';
		} else {
			$shutdown = 'register_shutdown_function';
		}

		$shutdown(function() use ($url) {
			if (is_callable('fastcgi_finish_request')) {
				@fastcgi_finish_request();
			}

			try {
				@file_get_contents($url, false,
					stream_context_create(['http'=>['timeout'=>1]])
				);
			} catch (Exception $e) {}
		});
	}

}
