<?php


namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING COMMON CONTENT/MIME TYPE TASKS
////////////////////////////////////////////////////////////////////////////////
class mime {


	////////////////////////////////////////////////////////////////////////////
	// CONTRUCTOR - PASS IN A FILE EXTENSION OR MIME TIME ID
	////////////////////////////////////////////////////////////////////////////
	public function __construct($type, \pudl $pudl=NULL) {
		$this->pudl = $pudl;

		if (is_numeric($type)) {
			$type = (int) $type;
		} else {
			$type = strtolower($type);
		}


		if ($pudl instanceof \pudl) {
			if (!array_key_exists($type, self::$cache)) {
				try {
					self::$cache[$type] = $pudl->cache(AF_DAY)->rowId(
						'mimetype',
						is_int($type) ? 'mime_id' : 'mime_ext',
						$type
					);
				} catch (\pudlException $e) {}
			}
		}


		// DEFAULT FALLBACK IF TABLE ISN'T INITIALIZED YET
		if (empty(self::$cache[$type])) {
			if (!empty($this::$defaults[$type])) {
				self::$cache[$type] = [
					'mime_ext'	=> $type,
					'mime_type'	=> $this::$defaults[$type],
				];
			} else {
				self::$cache[$type] = NULL;
			}
		}


		// STORE THE VALUE
		$this->type	= is_array(self::$cache[$type])
					? self::$cache[$type]
					: $type;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE MIME TYPE / CONTENT TYPE
	////////////////////////////////////////////////////////////////////////////
	public function __toString() {
		if (is_string($this->type)) return 'application/'.$this->type;
		if (!is_array($this->type)) return self::unknown;
		return (string) $this->type['mime_type'];
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE MIME TYPE / CONTENT TYPE
	////////////////////////////////////////////////////////////////////////////
	public function type() {
		return (string) $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE DATABASE ID
	////////////////////////////////////////////////////////////////////////////
	public function id() {
		if (is_string($this->type)) return NULL;
		if (!is_array($this->type)) return NULL;
		return (int) $this->type['mime_id'];
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CONVERTED FILE EXTENSION
	////////////////////////////////////////////////////////////////////////////
	public function ext() {
		if (is_string($this->type)) return $this->type;
		if (!is_array($this->type)) return NULL;

		return empty($this->type['af_ext'])
				? $this->type['mime_ext']
				: $this->type['af_ext'];
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE DATABASE CONNECTION THIS MIME TYPE IS ASSOCIATED WITH
	////////////////////////////////////////////////////////////////////////////
	public function pudl() {
		return $this->pudl;
	}




	////////////////////////////////////////////////////////////////////////////
	// PRIVATE LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private $pudl;
	private $type	=	NULL;




	////////////////////////////////////////////////////////////////////////////
	// PRIVATE STATIC VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private static $cache		= [];

	private static $defaults	= [
		'htm'		=>	'text/html',
		'html'		=>	'text/html',
		'css'		=>	'text/css',
		'js'		=>	'application/javascript',
		'txt'		=>	'text/plain',
		'jpg'		=>	'image/jpeg',
		'jpeg'		=>	'image/jpeg',
		'gif'		=>	'image/gif',
		'png'		=>	'image/png',
		'svg'		=>	'image/svg+xml',
		'json'		=>	'application/json',
		'xml'		=>	'text/xml',
	];




	////////////////////////////////////////////////////////////////////////////
	// LOCAL CONSTANTS
	////////////////////////////////////////////////////////////////////////////
	const unknown	=	'application/octet-stream';

}
