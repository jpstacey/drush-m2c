<?php

/**
 * @file
 * Drush make to Composer (m2c)
 */

/**
 * Implements hook_drush_help().
 */
function m2c_drush_help($section) {
  switch($section) {
    case 'drush:m2c':
      return dt("Drush make to Composer file conversion");
    break;
  }
}

/**
 * Implements hook_drush_command().
 */
function m2c_drush_command() {
  $items = array();
  $items['m2c'] = array(
    'description' => 'Convert a Drush makefile to composer format, on standard output',
    'arguments' => array(
      'makefile' => 'Makefile location',
    ),
    'options' => array(
      // 'name' => 'description',
    ),
    'aliases' => array('make2composer'),

    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  return $items;
}

/**
 * Implements hook_command_validate().
 *
 * Check makefile location provided, and that it exists.
 */
function drush_m2c_validate($makefile = NULL) {
  if (!$makefile) {
    drush_set_error("DRUSH_M2C_NO_MAKEFILE", "You must specify a makefile.");
    return;
  }
  if (!file_exists($makefile)) {
    drush_set_error("DRUSH_M2C_NO_MAKEFILE", "Makefile does not exist: '$make'.");
    return;
  }
}

/**
 * Drush command callback.
 */
function drush_m2c($makefile) {
  drush_include_engine("parsers", "default");

  $composer = m2c_parsers_default_m2c(
    m2c_parsers_default_internal($makefile)
  );

  print json_encode($composer, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES) . "\n";
}

/**
 * Implements hook_drush_engine_type_info().
 */
function m2c_drush_engine_type_info() {
  return array(
    'hacks' => array(),
    'parsers' => array(),
  );
}

/**
 * Implements hook_drush_engine_ENGINE().
 */
function m2c_drush_engine_hacks() {
  return array(
    'make' => array(
      'description' => "Hacks",
    ),
  );
}


/**
 * Implements hook_drush_engine_ENGINE().
 */
function m2c_drush_engine_parsers() {
  return array(
    'default' => array(
      'description' => "Parsers",
    ),
  );
}
