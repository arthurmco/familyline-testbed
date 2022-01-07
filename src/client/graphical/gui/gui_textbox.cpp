#include <client/graphical/gui/gui_textbox.hpp>

#include <fmt/core.h>
#include <string>

#include <cassert>
#include <cuchar>
#include <climits>

using namespace familyline::graphics::gui;

std::u32string GUITextbox::toU32(std::string s) const
{
    std::u32string ret{};
    ret.reserve(s.size());

    std::mbstate_t state{};
    char32_t codepoint;
    const char *ptr = s.c_str(), *end = s.c_str() + s.size() + 1;

    while(std::size_t rc = std::mbrtoc32(&codepoint, ptr, end - ptr, &state))
    {
        assert(rc != (std::size_t)-3); // no surrogates in UTF-32
        if(rc == (std::size_t)-1) break;
        if(rc == (std::size_t)-2) break;
        ret += codepoint;
        ptr += rc;
    }

    return ret;
}

  /// Convert a utf-32 string to a normal string
std::string GUITextbox::toNormalString(std::u32string text) const
{
    std::string ret;
    ret.reserve(text.size());
    
    std::mbstate_t state{};
    for (auto& codepoint : text) {        
        char c[MB_LEN_MAX+1] = {};
        auto rc = std::c32rtomb(c, codepoint, &state);                
        if(rc == (std::size_t)-1) continue;

        c[rc] = '\0';
        ret += c;
    }

    return ret;
    
}


std::string GUITextbox::text() const
{
    return toNormalString(text_);
}


/// A textual way of describing the control
/// If we were in Python, this would be its `__repr__` method
///
/// Used *only* for debugging purposes.
std::string GUITextbox::describe() const
{
  char v[128] = {};
  sprintf(v, "GUITextbox (id %08x, size %d x %d, pos %d,%d | text: '", id(),
          width_, height_, x_, y_);
  auto ret = std::string{v};
  ret += text();
  ret += "' ";

  if (parent_)
    ret += "has a parent ";

  if (onFocus_)
    ret += " | focus";

  ret += ")";

  return ret;
    
}

/**
 * Get the text data in selection blocks.
 *
 * Returns a tuple, where:
 * - the first element is the text before the selection
 * - the second element is the selected text, or "" if no text is selected.
 * - the third element is the text after the selection
 *
 * The 'block' parameter allows rendering a block cursor, where the cursor
 * 'selects' the current character.
 */
std::tuple<std::string, std::string, std::string> GUITextbox::getTextAsSelection(bool block) const
{
    auto begin = text_.substr(0, select_start_);
    auto selstart = select_start_;
    auto selend = select_end_;
    
    if (block) {
        selend++;
    }

    auto sel = text_.substr(selstart, selend - selstart);
    auto end = text_.substr(selend);
    
    if (block && end == U"")
        end = U"";
    
    return std::make_tuple(
        toNormalString(begin),
        toNormalString(sel),
        toNormalString(end)
     );    
}




void GUITextbox::receiveInput(const familyline::input::HumanInputAction &e) {
    using namespace familyline::input;

    if (auto *tev = std::get_if<TextInput>(&e.type); tev) {
        auto data32 = toU32(tev->text);
        text_.insert(select_end_, data32);
        select_end_ += data32.size();
        select_start_ = select_end_;
    } else if (auto *tev = std::get_if<TextEdit>(&e.type); tev) {
        fprintf(stderr, "eeee %s\n", tev->text);
    } else if (auto *kev = std::get_if<KeyAction>(&e.type); kev) {
        if (kev->keycode == SDLK_BACKSPACE && kev->isPressed) {
            fprintf(stderr, "backspace\n");
            if (select_start_ > 0) {
                text_.erase(select_start_-1, select_end_ - select_start_+1);
                select_start_--;
                select_end_ = select_start_;
            }
        } else if (kev->keycode == SDLK_DELETE && kev->isPressed) {
            if (select_end_ < text_.size()-1)
                text_.erase(select_start_, select_end_ - select_start_ + 1);
        }

        if (select_start_ == select_end_) {
            if (kev->keycode == SDLK_LEFT && kev->isPressed)
                select_end_ = std::max(0, int(select_end_)-1);
            else if (kev->keycode == SDLK_RIGHT && kev->isPressed)
                select_end_ = std::min(text_.size(), select_end_+1);

            select_start_ = select_end_;
        }
    } else if (auto *mev = std::get_if<ClickAction>(&e.type); mev) {
        select_start_ = getCharFromPosition(mev->screenX - x(), mev->screenY - y());
        select_end_ = select_start_;
    }

    
}

/**
 * Given a X and Y position relative to the start of the control,
 * give back the character position closest to that point
 */
size_t GUITextbox::getCharFromPosition(size_t x, size_t y)
{
    // charx = (charWidth_ * pixelx) / screenWidth_;
    // pixelx = ??

    // cx = (cw * px) / sw
    // cx * sw = cw * px
    // px = (cx * sw) / cw
    // = (1*800 / 110)


    // TODO: these sizes should come from the renderer.
    double borderToText = 1*800.0/110;
    double xoff = borderToText;
    double yoff = borderToText;

    int idx = 0;
    for (char32_t& c : text_) {
        auto gsize = this->render_info.getCodepointSize(c, appearance_.font,
                                                        appearance_.fontsize,
                                                        appearance_.weight);
        if (!gsize) {
            return idx;
        }
        
        xoff += gsize->width;
        if (xoff >= x) {
            return idx;
        }

        idx++;        
    }

    return idx-1;
}
