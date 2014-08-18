<?php

/**
 * @file
 * Engine: parsers, default.
 */

/**
 * Parse a makefile into the internal format provided by make.
 */
function m2c_parsers_default_internal($makefile) {
  drush_include_engine("hacks", "make");

  // Parse .info file into nested arrays.
  $make_info = make_parse_info_file($makefile);

  // Convert this nested arrays into an internal format, at the same time
  // looking up any metadata for all projects for which it is missing.
  $internal_make_info = _make_projects_hacked(FALSE, "/tmp", $make_info, "/tmp", "/tmp");
  // Merge libraries in
  $internal_make_info['libraries'] = _make_libraries_hacked("/tmp", $make_info, "/tmp", "/tmp");

  return $internal_make_info;
}

/**
 * Parse internal make format into composer
 */
function m2c_parsers_default_m2c($internal_make_info) {
  // Start composer file.
  $composer = array(
    'repositories' => array(
    ),
    'require' => array(
      'composer/installers' => '~1.0',
    ),
  );

  foreach($internal_make_info as $supertype => $packages) {
    switch($supertype) {
      case "contrib":
        foreach($packages as $name => $package) {
          // Package version can be empty, even after autodiscovery.
          if ($package["version"] == "") {
            $package["version"] = str_replace($package["core"] . "-", "", $package['download']['full_version']);
          }

          // Assemble a composer package structure.
          $composer_package = array(
            "name" => "drupal/$name",
            "type" => "drupal-" . $package['type'],
            // Version not always defined in original makefile; we can't use "core",
            // because Composer version numbers cannot end ".x".
            "version" => $package['version'] ? $package['version'] : "0.111",
          );

          // Get dist/source based on download method.
          $composer_package += _m2c_map_download_to_composer_dist($package['download'], $package);

          // Put package in as a package repository.
          $composer['repositories'][$name] = array(
            "type" => "package",
            "package" => $composer_package,
          );
          // Then reference it as a requirement, matching the package version.
          $composer['require'][$composer_package['name']] = $composer_package['version'];
        }
        break;

      case "libraries":
        foreach($packages as $name => $package) {
          // Libraries have no version that Drupal cares about.
          $package['version'] = "1.0";

          // Assemble a composer package structure.
          $composer_package = array(
            "name" => "library/$name",
            "version" => $package['version'],
            "type" => "drupal-" . $package['type'],
          );
          $composer_package += _m2c_map_download_to_composer_dist($package['download']);

          // Put package in as a package repository, avoiding name clash
          // with any Drupal projects e.g. colorbox, flexslider etc.
          $composer['repositories']["libraries/$name"] = array(
            "type" => "package",
            "package" => $composer_package,
          );
          $composer['require'][$composer_package['name']] = $composer_package['version'];
        }
        break;

      default:
        drush_log("Didn't deal with make section $supertype", "warning");
    }
  }

  return $composer;
}

/**
 * Helper function to map a download array to a composer source/dist.
 *
 * @param array $download
 *   the download array from the makefile package
 * @param array $package
 *   optional array of full package data, where autodiscovery is still required.
 *
 * @return array
 *   dist/source data for composer.
 */
function _m2c_map_download_to_composer_dist($download, $package = NULL) {
  $url = "";
  foreach(array("url", "download_link") as $key) {
    if (array_key_exists($key, $download)) {
      $url = $download[$key];
    }
  }

  // Without a download type, just assume HTTP GET.kj
  if (!isset($download['type'])) {
    $download['type'] = "get";
  }

  switch($download['type']) {
    // Drush pm always returns a gzip file.
    case "pm":
      // Assume "gzip"
      return array(
        "dist" => array(
          "url" => $url,
          "type" => "gzip",
        ),
      );

    // Standard HTTP GET, possibly mediated by drush pm.
    case "get":
      // Assume "zip"
      return array(
        "dist" => array(
          "url" => $url,
          "type" => "zip",
        ),
      );

    case "file":
      // Does Composer even have a "file" dist type?
      return array(
        "dist" => array(
          "url" => $url,
          "type" => "file",
        ),
      );


    case "git":
      // Work out the git reference, in Drush-make precedence order.
      $reference = "master";
      foreach (array("tag", "branch", "revision") as $commitish) {
        if (isset($download[$commitish])) {
          $reference = $download[$commitish];
          break;
        }
      }

      // Auto-guess a URL if none given, asssuming it's a Drupal project.
      if (!$url && isset($package)) {
        $url = "http://git.drupal.org/project/" . $package['name'] . ".git";
      }

      // => git.
      return array(
        "source" => array(
          "url" => $url,
          "type" => "git",
          "reference" => $reference,
        ),
      );

    default:
      // Report error as an exception.
      $type = $download['type'];
      unset($download['type']);
      throw new Exception(
        "Unknown make download type $type; more info: " . var_export($download, TRUE)
      );
  }
}
