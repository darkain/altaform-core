<?php

require_once '_amazon/aws.phar';

use Aws\S3\S3Client;
use Aws\S3\Enum\Group;
use Aws\S3\Enum\CannedAcl;
use Aws\Common\Enum\Region;


function s3client() {
	global $afconfig;

	static $client = false;

	if (empty($client)) $client = S3Client::factory([
		'key'		=> $afconfig->amazon['id'],
		'secret'	=> $afconfig->amazon['secret'],
		'base_url'	=> $afconfig->amazon['url'],
	]);

	return $client;
}



// Upload an object by streaming the contents of a file
// $filepath should be absolute path to a file on disk
function s3delete($bucket, $awspath, $options=[]) {
	$client = s3client();
	return $client->deleteObject(array_merge($options, [
		'Bucket'	=> $bucket,
		'Key'		=> $awspath,
	]));
}



function s3upload($filepath, $bucket, $awspath, $mimetype=false, $options=[]) {
	global $db;

	//If $mimetype is an extension, pull the mimetype from database
	if (is_string($mimetype)  &&  !strpos($mimetype, '/')) {
		$mimetype = $db->cellId('pudl_mimetype', 'mime_type', 'mime_ext', $mimetype);
	}

	if (empty($mimetype)) $mimetype = 'application/octet-stream';


	// Upload an object by streaming the contents of a file
	// $filepath should be absolute path to a file on disk
	$client = s3client();
	$result = $client->putObject(array_merge($options, [
		'SourceFile'	=> $filepath,
		'Bucket'		=> $bucket,
		'Key'			=> $awspath,
		'ContentType'	=> $mimetype,
		'acl'			=> CannedAcl::PUBLIC_READ,
		'GrantRead'		=> 'uri="' . Group::ALL_USERS . '"',
	]));

	// We can poll the object until it is accessible
	$client->waitUntilObjectExists(['Bucket'=>$bucket, 'Key'=>$awspath]);

	return $result;
}



function s3list($bucket, $start='') {
	$client = s3client();
	$iterator = $client->getIterator('ListObjects', [
		'Bucket' => $bucket,
		'Marker' => $start,
	]);

	$list = [];
	foreach ($iterator as $object) $list[] = $object['Key'];
	return $list;
}
