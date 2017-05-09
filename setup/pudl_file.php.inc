<?php

/*

CREATE TABLE IF NOT EXISTS `pudl_mimetype` (
  `mime_id` smallint(5) UNSIGNED NOT NULL AUTO_INCREMENT,
  `mime_ext` char(10) CHARACTER SET utf8 NOT NULL,
  `mime_type` varchar(100) CHARACTER SET utf8 NOT NULL,
  `af_ext` char(4) CHARACTER SET utf8 DEFAULT NULL,
  PRIMARY KEY (`mime_id`),
  UNIQUE KEY `mime_ext` (`mime_ext`),
  KEY `mime_type` (`mime_type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;


SET SESSION sql_mode='NO_AUTO_VALUE_ON_ZERO';

INSERT INTO `pudl_mimetype` (`mime_id`, `mime_ext`, `mime_type`, `af_ext`) VALUES
(0, '', 'application/octet-stream'),
(1, 'xhtml', 'application/xhtml+xml'),
(2, 'htm', 'text/htm', 'html'),
(3, 'html', 'text/html'),
(4, 'sht', 'text/html'),
(5, 'shtm', 'text/html'),
(6, 'shtml', 'text/html'),
(7, 'stm', 'text/html'),
(8, 'wap', 'text/html'),
(9, 'css', 'text/css'),
(10, 'js', 'application/javascript'),
(11, 'xml', 'text/xml'),
(12, 'txt', 'text/plain'),
(13, 'rss', 'application/rss+xml'),
(14, 'swf', 'application/x-shockwave-flash'),
(15, 'bas', 'text/plain'),
(16, 'c', 'text/plain'),
(17, 'cs', 'text/plain'),
(18, 'cpp', 'text/plain'),
(19, 'h', 'text/plain'),
(20, 'pls', 'text/plain'),
(21, 'uls', 'text/iuls'),
(22, 'rtx', 'text/richtext'),
(23, 'sct', 'text/scriptlet'),
(24, 'tsv', 'text/tab-separated-values'),
(25, 'htt', 'text/webviewhtml'),
(26, '323', 'text/h323'),
(27, 'htc', 'text/x-component'),
(28, 'etx', 'text/x-setext'),
(29, 'vcf', 'text/x-vcard'),
(30, 'jpe', 'image/jpeg', 'jpg'),
(31, 'jpg', 'image/jpeg'),
(32, 'jpeg', 'image/jpeg', 'jpg'),
(33, 'tif', 'image/tiff'),
(34, 'tiff', 'image/tiff', 'tif'),
(35, 'gif', 'image/gif'),
(36, 'png', 'image/png'),
(37, 'bmp', 'image/bmp'),
(38, 'ico', 'image/x-icon'),
(39, 'flr', 'x-world/x-vrml'),
(40, 'vrml', 'x-world/x-vrml'),
(41, 'wrl', 'x-world/x-vrml'),
(42, 'wrz', 'x-world/x-vrml'),
(43, 'xaf', 'x-world/x-vrml'),
(44, 'xof', 'x-world/x-vrml'),
(45, 'svg', 'image/svg+xml'),
(46, 'ief', 'image/ief'),
(47, 'cod', 'image/cis-cod'),
(48, 'jfif', 'image/pipeg'),
(49, 'pnm', 'image/x-portable-anymap'),
(50, 'pbm', 'image/x-portable-bitmap'),
(51, 'pgm', 'image/x-portable-graymap'),
(52, 'ppm', 'image/x-portable-pixmap'),
(53, 'ras', 'image/x-cmu-raster'),
(54, 'rgb', 'image/x-rgb'),
(55, 'xbm', 'image/x-xbitmap'),
(56, 'xpm', 'image/x-xpixmap'),
(57, 'xwd', 'image/x-xwindowdump'),
(58, 'psd', 'image/vnd.adobe.photoshop'),
(59, 'gz', 'application/octet-stream'),
(60, 'tar', 'application/octet-stream'),
(61, 'rar', 'application/x-rar-compressed'),
(62, 'bin', 'application/octet-stream'),
(63, 'class', 'application/octet-stream'),
(64, 'dms', 'application/octet-stream'),
(65, 'exe', 'application/octet-stream'),
(66, 'lha', 'application/octet-stream'),
(67, 'lzh', 'application/octet-stream'),
(68, 'com', 'application/octet-stream'),
(69, 'doc', 'application/msword'),
(70, 'dot', 'application/msword'),
(71, 'docx', 'application/vnd.openxmlformats-officedocument.wordprocessingml.document'),
(72, 'dotx', 'application/vnd.openxmlformats-officedocument.wordprocessingml.template'),
(73, 'docm', 'application/vnd.ms-word.document.macroEnabled.12'),
(74, 'dotm', 'application/vnd.ms-word.template.macroEnabled.12'),
(75, 'xls', 'application/vnd.ms-excel'),
(76, 'xlt', 'application/vnd.ms-excel'),
(77, 'xla', 'application/vnd.ms-excel'),
(78, 'xlsx', 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'),
(79, 'xltx', 'application/vnd.openxmlformats-officedocument.spreadsheetml.template'),
(80, 'xlsm', 'application/vnd.ms-excel.sheet.macroEnabled.12'),
(81, 'xltm', 'application/vnd.ms-excel.template.macroEnabled.12'),
(82, 'xlam', 'application/vnd.ms-excel.addin.macroEnabled.12'),
(83, 'xlsb', 'application/vnd.ms-excel.sheet.binary.macroEnabled.12'),
(84, 'ppt', 'application/vnd.ms-powerpoint'),
(85, 'pot', 'application/vnd.ms-powerpoint'),
(86, 'pps', 'application/vnd.ms-powerpoint'),
(87, 'ppa', 'application/vnd.ms-powerpoint'),
(88, 'pptx', 'application/vnd.openxmlformats-officedocument.presentationml.presentation'),
(89, 'potx', 'application/vnd.openxmlformats-officedocument.presentationml.template'),
(90, 'ppsx', 'application/vnd.openxmlformats-officedocument.presentationml.slideshow'),
(91, 'ppam', 'application/vnd.ms-powerpoint.addin.macroEnabled.12'),
(92, 'pptm', 'application/vnd.ms-powerpoint.presentation.macroEnabled.12'),
(93, 'potm', 'application/vnd.ms-powerpoint.template.macroEnabled.12'),
(94, 'ppsm', 'application/vnd.ms-powerpoint.slideshow.macroEnabled.12'),
(95, 'json', 'application/json'),
(96, 'jsonp', 'application/javascript'),
(97, 'csv', 'text/csv'),
(98, 'woff', 'font/woff'),
(99, 'woff2', 'font/woff2'),
(100, 'ttf', 'application/font-sfnt'),
(101, 'otf', 'application/font-sfnt');



CREATE TABLE `pudl_file` (
  `file_hash` binary(16) NOT NULL COMMENT 'MD5 hash value of file contents',
  `file_parent` binary(16) DEFAULT NULL,
  `file_size` bigint(10) UNSIGNED NOT NULL COMMENT 'File size on disc, in bytes',
  `file_uploaded` int(10) UNSIGNED NOT NULL COMMENT 'UNIX timestamp of file upload time',
  `file_visible` set('admin','staff','debug','user','jobber','dealer','wholesale') COLLATE ascii_bin NOT NULL DEFAULT 'admin,staff,debug,user,jobber,dealer,wholesale',
  `mime_id` int(11) DEFAULT NULL,
  `file_name` varchar(255) CHARACTER SET ascii NOT NULL,
  PRIMARY KEY (`file_hash`),
  KEY `mime_id` (`mime_id`),
  KEY `file_name` (`file_name`),
  KEY `file_parent` (`file_parent`),
  CONSTRAINT `pudl_file_mime` FOREIGN KEY (`mime_id`) REFERENCES `pudl_mimetype` (`mime_id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `pudl_file_parent` FOREIGN KEY (`file_parent`) REFERENCES `pudl_file` (`file_hash`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin;


CREATE TABLE `pudl_file_thumb` (
  `thumb_hash` binary(16) NOT NULL,
  `file_hash` binary(16) NOT NULL,
  `thumb_size` int(10) UNSIGNED NOT NULL,
  `thumb_type` enum('50','100','150','200','800','1280','1920') CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
  PRIMARY KEY (`thumb_hash`),
  KEY `file_hash` (`file_hash`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


ALTER TABLE `pudl_mimetype`
  ADD CONSTRAINT `pudl_mime_icon` FOREIGN KEY (`mime_icon`) REFERENCES `pudl_file` (`file_hash`) ON DELETE SET NULL ON UPDATE CASCADE;
*/


$db(
	'CREATE TABLE ' . $db->_table('pudl_file_meta') . ' (
	  `file_hash` binary(16) NOT NULL,
	  `file_meta_name` varchar(32) COLLATE utf8_bin NOT NULL,
	  `file_meta_value` varchar(255) COLLATE utf8_bin NOT NULL,
	  PRIMARY KEY (`file_hash`,`file_meta_name`),
	  KEY `file_meta_name` (`file_meta_name`),
	  CONSTRAINT `pudl_file_meta` FOREIGN KEY (`file_hash`) REFERENCES ' . $db->_table('pudl_file') . ' (`file_hash`) ON DELETE CASCADE ON UPDATE CASCADE
	) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin'
);


/*

CREATE TABLE IF NOT EXISTS `pudl_file_user` (
  `file_hash` binary(16) NOT NULL,
  `user_id` int(10) UNSIGNED NOT NULL,
  `user_time` bigint(20) NOT NULL,
  `file_credit_by` int(10) UNSIGNED DEFAULT NULL,
  `file_user_visible` tinyint(1) NOT NULL DEFAULT '1',
  `file_text` text NOT NULL,
  PRIMARY KEY (`file_hash`,`user_id`),
  KEY `user_id` (`user_id`),
  KEY `file_credit_by` (`file_credit_by`),
  KEY `user_time` (`user_time`),
  CONSTRAINT `pudl_file_user_hash` FOREIGN KEY (`file_hash`) REFERENCES `pudl_file` (`file_hash`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `pudl_file_user_id` FOREIGN KEY (`user_id`) REFERENCES `pudl_user` (`user_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `pudl_file_credit` FOREIGN KEY (`file_credit_by`) REFERENCES `pudl_user` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

*/
