#ifndef _Spline_h
#define _Spline_h

class Spline {

public:
    Spline();
    void SetStart (int, int);
    void SetEnd (int, int);
    void Render(SDL_Renderer **ren) const;
    bool HasStart() const;
    bool HasEnd() const;
    void getStart(int& x1_, int& y1_);
    void getEnd(int& x2_, int& y2_);
    void setRGB(int r_, int g_, int b_);

private:
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    int r,g,b;
};

#endif
