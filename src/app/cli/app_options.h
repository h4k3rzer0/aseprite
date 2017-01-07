// Aseprite
// Copyright (C) 2001-2016  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_CLI_APP_OPTIONS_H_INCLUDED
#define APP_CLI_APP_OPTIONS_H_INCLUDED
#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "base/program_options.h"

namespace app {

class AppOptions {
public:
  enum VerboseLevel {
    kNoVerbose,
    kVerbose,
    kHighlyVerbose,
  };

  typedef base::ProgramOptions PO;
  typedef PO::Option Option;
  typedef PO::ValueList ValueList;

  AppOptions(int argc, const char* argv[]);

  const std::string& exeName() const { return m_exeName; }
  const base::ProgramOptions& programOptions() const { return m_po; }

  bool startUI() const { return m_startUI; }
  bool startShell() const { return m_startShell; }
  bool previewCLI() const { return m_previewCLI; }
  bool showHelp() const { return m_showHelp; }
  bool showVersion() const { return m_showVersion; }
  VerboseLevel verboseLevel() const { return m_verboseLevel; }

  const ValueList& values() const {
    return m_po.values();
  }

  // Export options
  const Option& saveAs() const { return m_saveAs; }
  const Option& palette() const { return m_palette; }
  const Option& scale() const { return m_scale; }
  const Option& shrinkTo() const { return m_shrinkTo; }
  const Option& data() const { return m_data; }
  const Option& format() const { return m_format; }
  const Option& sheet() const { return m_sheet; }
  const Option& sheetWidth() const { return m_sheetWidth; }
  const Option& sheetHeight() const { return m_sheetHeight; }
  const Option& sheetType() const { return m_sheetType; }
  const Option& sheetPack() const { return m_sheetPack; }
  const Option& splitLayers() const { return m_splitLayers; }
  const Option& splitTags() const { return m_splitTags; }
  const Option& layer() const { return m_layer; }
  const Option& allLayers() const { return m_allLayers; }
  const Option& ignoreLayer() const { return m_ignoreLayer; }
  const Option& frameTag() const { return m_frameTag; }
  const Option& frameRange() const { return m_frameRange; }
  const Option& ignoreEmpty() const { return m_ignoreEmpty; }
  const Option& borderPadding() const { return m_borderPadding; }
  const Option& shapePadding() const { return m_shapePadding; }
  const Option& innerPadding() const { return m_innerPadding; }
  const Option& trim() const { return m_trim; }
  const Option& crop() const { return m_crop; }
  const Option& filenameFormat() const { return m_filenameFormat; }
#ifdef ENABLE_SCRIPTING
  const Option& script() const { return m_script; }
#endif
  const Option& listLayers() const { return m_listLayers; }
  const Option& listTags() const { return m_listTags; }
  const Option& oneFrame() const { return m_oneFrame; }

  bool hasExporterParams() const;

private:
  std::string m_exeName;
  base::ProgramOptions m_po;
  bool m_startUI;
  bool m_startShell;
  bool m_previewCLI;
  bool m_showHelp;
  bool m_showVersion;
  VerboseLevel m_verboseLevel;

#ifdef ENABLE_SCRIPTING
  Option& m_shell;
#endif
  Option& m_batch;
  Option& m_preview;
  Option& m_saveAs;
  Option& m_palette;
  Option& m_scale;
  Option& m_shrinkTo;
  Option& m_data;
  Option& m_format;
  Option& m_sheet;
  Option& m_sheetWidth;
  Option& m_sheetHeight;
  Option& m_sheetType;
  Option& m_sheetPack;
  Option& m_splitLayers;
  Option& m_splitTags;
  Option& m_layer;
  Option& m_allLayers;
  Option& m_ignoreLayer;
  Option& m_frameTag;
  Option& m_frameRange;
  Option& m_ignoreEmpty;
  Option& m_borderPadding;
  Option& m_shapePadding;
  Option& m_innerPadding;
  Option& m_trim;
  Option& m_crop;
  Option& m_filenameFormat;
#ifdef ENABLE_SCRIPTING
  Option& m_script;
#endif
  Option& m_listLayers;
  Option& m_listTags;
  Option& m_oneFrame;

  Option& m_verbose;
  Option& m_debug;
  Option& m_help;
  Option& m_version;

};

} // namespace app

#endif
