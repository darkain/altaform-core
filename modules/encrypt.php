<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// ENCRYPT OR DECRYPT INFO
////////////////////////////////////////////////////////////////////////////////
trait encrypt {




	////////////////////////////////////////////////////////////////////////////
	// ENCRYPT A STRING, RETURNING AN ARRAY
	////////////////////////////////////////////////////////////////////////////
	public function encrypt($data) {
		$this->checkExtension('openssl');

		if (empty(		$this->config->encrypt['cipher'])	||
			empty(		$this->config->encrypt['key'])		||
			is_null(	$this->{'af.encrypt'})) {
			throw new exception\config('Encryption cipher or key not set');
		}

		$data		=	json_encode($data);
		$ivlen		=	openssl_cipher_iv_length($this->config->encrypt['cipher']);
		$iv			=	openssl_random_pseudo_bytes($ivlen);
		$hash		=	hash_hmac('sha256', $data, $iv);

		return [
			'iv'	=>	rtrim(base64_encode($iv), '='),
			'raw'	=>	rtrim(openssl_encrypt(
							$hash . ':' . $data,
							$this->config->encrypt['cipher'],
							$this->config->encrypt['key'] . $this->{'af.encrypt'},
							$options=0,
							$iv
						), '='),
		];
	}




	////////////////////////////////////////////////////////////////////////////
	// DECRYPT A PREVIOUSLY ENCRYPTED STRING
	////////////////////////////////////////////////////////////////////////////
	public function decrypt($encrypted) {
		$this->checkExtension('openssl');

		if (empty($this->config->encrypt['cipher'])  ||
			empty($this->config->encrypt['key'])  ||
			is_null($this->{'af.encrypt'})) {
			throw new afException('Encryption cipher or key not set');
		}

		if (empty($encrypted['iv'])  ||  empty($encrypted['raw'])) return NULL;

		$iv			= @base64_decode($encrypted['iv']);

		$decrypted	= @openssl_decrypt(
			$encrypted['raw'],
			$this->config->encrypt['cipher'],
			$this->config->encrypt['key'] . $this->{'af.encrypt'},
			$options=0,
			$iv
		);

		@list($hash, $string) = explode(':', $decrypted, 2);

		return (hash_equals($hash, hash_hmac('sha256', $string, $iv)))
				? @json_decode($string, true)
				: NULL;
	}


}
