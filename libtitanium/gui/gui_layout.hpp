#pragma once

#include <libtitanium/util/maths.hpp>
#include <libtitanium/util/data/multitree.hpp>

#include <libtitanium/renderer/renderer.hpp>

/*
 *  The gui system essentially defines pure functions that take user input and code-defined parameters (usually position and such), render stuff, and can potentially call user code
 *  gui elements are stored in a tree of parents/children, this is mainly so we can avoid redrawing stuff if it hasnt updated
 *
 *  gui elements can be created in two modes, immediate mode or persistent mode
 *  immediate mode elements are created by code, they exist only on frames where their creation function is called and doesn't require their state to be managed manually
 *  immediate mode elements are (by design) usually significantly slower to use than persistent mode ones (though not really by too much)
 *  they require hashtable lookups every frame, have to have their state changes manually diffed across frames, and have to be garbage collected
 *  you should really only use immediate stuff for debugging purposes or (potentially) stuff like player hud that you know will be displayed basically all the time
 *
 *  persistent elements are faster and are a fair bit simpler than immediate stuff
 *  persistent elements require manual state management by code, and need to be manually created and destroyed - no garbage collection!
 */


/*
 *  ideas:
 *  coordinate system: annoying!!
 *  should probably be relative to bounds of parent panel (so, 0,0 = bottom left of parent panel)
 *  annoying thing is how to deal with stuff stretching if panel isnt square (most panels probably won't be square anyway, bleh)
 *  element scaling should probably happen based on the lower of either the width or height of the panel (so on 16:9 we scale by height, on 9:16 we scale by width)
 *  maybe multiple scaling modes? like for instance
 *  annoying thing also: lots of the time we probably want to scale an object's position, but not its width/height??? blehhhhhhhhhhhhhh!!!
 */

namespace gui
{
    struct ElementData
    {
        util::maths::Vec2<f32> relativePosition;

        struct {
            util::maths::Vec2<f32> mins;
            util::maths::Vec2<f32> maxs;
        } bounds;

        GPUTexture gpuTexture;
    };

    using ElementNode = util::data::MultiTree<ElementData>;
}
