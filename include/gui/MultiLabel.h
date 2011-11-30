/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/

#if !defined( __MULTILABEL_H__ )
#define __MULTILABEL_H__

#include "Control.h"
#include "Label.h"
#include "TextManager.h"
#include "Button.h"
#include <vector>
//class cTextElement;

// #include "../Fluid/mmgr.h"

#define ALIGN_LEFT  0
#define ALIGN_CENTER  1
#define ALIGN_RIGHT  2


#define STYLE_NORMAL	0x00
#define STYLE_BOLD		0x01
#define STYLE_ITALIC	0x02
#define STYLE_UNDERLINE	0x04
//using namespace std;

struct MultiLabelComponent
{
 int line;
 //int hue;
 int style;
 int r;
 int g;
 int b;
 int font;
 int alignment;
 int x_fix;
 std::string href;
 std::string text;
};

typedef std::vector<MultiLabelComponent> __line;

class cMultiLabel : public Control
{
    
    private:
        unsigned int _align;
        __line line;
        std::vector<__line> lines;
        std::vector<Label*> labels;
        //std::vector<int> lines_textlen;
        int _width;
        int _height;
        int _x;
        int _y;
  
        int _scrollbar;
        Button * scrollup;
        Button * scrolldown;
        bool scrollbuts_created;
        
        //int onscrollup(Control * contr);
        //int onscrolldown(Control * contr);
     
    
    public:
        cMultiLabel(int x, int y, int width, int height, int scrollbar=false);
        ~cMultiLabel();
        void AddLabel(MultiLabelComponent component);
        void AddLine();
        void Draw( GumpHandler *gumps );
        void Create();
        void ScrollUp();
        void ScrollDown();

        

    
};

#endif
