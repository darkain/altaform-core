<?php

trait af_node {

	function push($message) {
		global $af, $afurl;

		if (empty($afurl->push)) return;

		$message = json_encode($message);

		$url = $afurl->push . '?' . http_build_query([
			'time'		=> $af->time(),
			'auth'		=> $af->config->auth($message),
			'user'		=> (int) $this->user_id,
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
