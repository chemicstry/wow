DROP TABLE IF EXISTS `cheat_first_report`;
CREATE TABLE `cheat_first_report` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `guid` int(11) unsigned NOT NULL,
  `name` varchar(100) collate utf8_unicode_ci default NULL,
  `time` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `guid` (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

DROP TABLE IF EXISTS `cheat_reports`;
CREATE TABLE `cheat_reports` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `guid` int(11) unsigned NOT NULL,
  `name` varchar(100) default NULL,
  `mapid` smallint(5) unsigned NOT NULL default '0',
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `report` varchar(100) default NULL,
  `time` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `guid` (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

DROP TABLE IF EXISTS `cheat_temp_reports`;
CREATE TABLE `cheat_temp_reports` (
  `id` int(11) unsigned NOT NULL auto_increment,
  `guid` int(11) unsigned NOT NULL,
  `name` varchar(100) default NULL,
  `mapid` smallint(5) unsigned NOT NULL default '0',
  `position_x` float NOT NULL default '0',
  `position_y` float NOT NULL default '0',
  `position_z` float NOT NULL default '0',
  `report` varchar(100) default NULL,
  `time` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `guid` (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

REPLACE INTO `command` (`name`,`security`,`help`) VALUES
('anticheat global', '2', 'Syntax: .anticheat global returns the total amount reports and the average. (top three players)'),
('anticheat player', '2', 'Syntax: .anticheat player $name returns the players''s total amount of warnings, the average and the amount of each cheat type.'),
('anticheat handle', '2', 'Syntax: .anticheat handle [on|off] Turn on/off the AntiCheat-Detection .'),
('anticheat delete', '2', 'Syntax: .anticheat delete [deleteall|$name] Deletes the report records of all the players or deletes all the reports of player $name.');