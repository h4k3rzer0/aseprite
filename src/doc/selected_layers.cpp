// Aseprite Document Library
// Copyright (c) 2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "doc/selected_layers.h"

#include "doc/layer.h"
#include "doc/sprite.h"

namespace doc {

void SelectedLayers::clear()
{
  m_set.clear();
}

void SelectedLayers::insert(Layer* layer)
{
  ASSERT(layer);
  m_set.insert(layer);
}

void SelectedLayers::erase(Layer* layer)
{
  m_set.erase(layer);
}

bool SelectedLayers::contains(Layer* layer) const
{
  return m_set.find(layer) != m_set.end();
}

bool SelectedLayers::hasSameParent() const
{
  Layer* parent = nullptr;
  for (auto layer : *this) {
    if (parent) {
      if (layer->parent() != parent)
        return false;
    }
    else
      parent = layer->parent();
  }
  return true;
}

LayerList SelectedLayers::toLayerList() const
{
  LayerList output;

  if (empty())
    return output;

  ASSERT(*begin());
  ASSERT((*begin())->sprite());

  for (Layer* layer = (*begin())->sprite()->firstBrowsableLayer();
       layer != nullptr;
       layer = layer->getNextBrowsable()) {
    if (contains(layer))
      output.push_back(layer);
  }

  return output;
}

void SelectedLayers::removeChildrenIfParentIsSelected()
{
  SelectedLayers removeThese;

  for (Layer* child : *this) {
    Layer* parent = child->parent();
    while (parent) {
      if (contains(parent)) {
        removeThese.insert(child);
        break;
      }
      parent = parent->parent();
    }
  }

  for (Layer* child : removeThese)
    erase(child);
}

void SelectedLayers::selectAllLayers(LayerGroup* group)
{
  for (Layer* layer : group->layers()) {
    if (layer->isGroup())
      selectAllLayers(static_cast<LayerGroup*>(layer));
    insert(layer);
  }
}

void SelectedLayers::displace(layer_t layerDelta)
{
  // Do nothing case
  if (layerDelta == 0)
    return;

  const SelectedLayers original = *this;

retry:;
  clear();

  for (auto it : original) {
    Layer* layer = it;

    if (layerDelta > 0) {
      for (layer_t i=0; layer && i<layerDelta; ++i)
        layer = layer->getNextBrowsable();
    }
    else if (layerDelta < 0) {
      for (layer_t i=0; layer && i>layerDelta; --i) {
        layer = layer->getPreviousBrowsable();
      }
    }

    if (layer) {
      insert(layer);
    }
    // If some layer is outside the range it means that the delta is
    // too big (out of bounds), we reduce the delta and try again the
    // whole process.
    else {
      layerDelta -= SGN(layerDelta);
      if (layerDelta == 0) {
        m_set = original.m_set;
        break;
      }
      goto retry;
    }
  }
}

// This will select:
// 1. all visible children in case the parent is selected and none of
//    its children is selected.
// 2. all parent if one children is selected
void SelectedLayers::propagateSelection()
{
  SelectedLayers newSel;

  for (Layer* layer : *this) {
    if (!layer->isGroup())
      continue;

    LayerList children;
    static_cast<LayerGroup*>(layer)->allLayers(children);

    bool allDeselected = true;
    for (Layer* child : children) {
      if (contains(child)) {
        allDeselected = false;
        break;
      }
    }
    if (allDeselected) {
      for (Layer* child : children) {
        if (child->isVisible())
          newSel.insert(child);
      }
    }
  }

  for (Layer* layer : *this) {
    Layer* parent = layer->parent();
    while (parent != layer->sprite()->root() &&
           !contains(parent)) {
      newSel.insert(parent);
      parent = parent->parent();
    }
  }

  for (Layer* layer : newSel)
    insert(layer);
}

} // namespace doc
