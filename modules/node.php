<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS TO INTERACT WITH ALTAFORM NODE.JS SERVER
////////////////////////////////////////////////////////////////////////////////
trait node {




	////////////////////////////////////////////////////////////////////////////
	// SEND A MESSAGE TO OUR CUSTOM NODE.JS SERVER TO RELAY BACK TO CLIENT
	////////////////////////////////////////////////////////////////////////////
	function node($message) {
		if (empty($this->af->url->push)) return;

		$message = json_encode($message);

		$url = $this->af->url->push . '?' . $this->af->url->query([
			'time'		=> $this->af->time(),
			'auth'		=> $this->af->config->auth($message),
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
