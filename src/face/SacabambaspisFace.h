#ifndef FACES_SACABAMBASPISFACE_H_
#define FACES_SACABAMBASPISFACE_H_

#include <M5Unified.h>
#include <Avatar.h>

namespace m5avatar
{

  /**
   * 目のクラス
   */
  class SacabambaspisEye : public Drawable
  {

  private:
    uint16_t r;
    bool isLeft;

  public:
    SacabambaspisEye(uint16_t r, bool isLeft)
        : r{r}, isLeft{isLeft} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      Expression exp = ctx->getExpression();
      uint32_t x = rect.getCenterX();
      uint32_t y = rect.getCenterY();
      Gaze g = ctx->getLeftGaze();
      float openRatio = ctx->getLeftEyeOpenRatio();
      uint32_t offsetX = g.getHorizontal() * 3;
      uint32_t offsetY = g.getVertical() * 3;
      ColorPalette *cp = ctx->getColorPalette();
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
      uint16_t backgroundColor = ctx->getColorDepth() == 1 ? ERACER_COLOR : cp->get(COLOR_BACKGROUND);

      if (openRatio > 0)
      {
        // Happy, Angry, Sad, Doubt, Sleepy, Neutral
        if (exp == Expression::Happy)
        {
          // spi->fillCircle(x + offsetX, y + offsetY, r, primaryColor);
          int inner12 = 10;
          int inner3 = 15;
          if (!isLeft)
          {
            int rx = x + 10;
            spi->fillTriangle(rx - r + offsetX, y - r + offsetY, rx - r + offsetX, y + r + offsetY, rx + r + offsetX, y + offsetY, primaryColor);
            spi->fillTriangle(rx - r + offsetX, y - r + offsetY + inner12, rx - r + offsetX, y + r + offsetY - inner12, rx + r + offsetX - inner3, y + offsetY, TFT_LIGHTGRAY);
          }
          else
          {
            int lx = x - 10;
            spi->fillTriangle(lx + r + offsetX, y - r + offsetY, lx + r + offsetX, y + r + offsetY, lx - r + offsetX, y + offsetY, primaryColor);
            spi->fillTriangle(lx + r + offsetX, y - r + offsetY + inner12, lx + r + offsetX, y + r + offsetY - inner12, lx - r + offsetX + inner3, y + offsetY, TFT_LIGHTGRAY);
          }
        }
        else if (exp == Expression::Angry)
        {
          spi->fillCircle(x + offsetX, y + offsetY, r, primaryColor);
          spi->fillCircle(x + offsetX, y + offsetY, r*0.9, backgroundColor);
          spi->fillCircle(x + offsetX, y + offsetY, r*0.6, TFT_RED);
        }
        else if (exp == Expression::Sad)
        {
          spi->fillRect(x - 5, y - 13, 10, 10, primaryColor);
          spi->fillRect(x - 5, y + 3 + 1, 10, 10, primaryColor);
          spi->fillRect(x - 5, y + 3 + 10 + 1, 10, 8, primaryColor);
          spi->fillRect(x - 5, y + 3 + 10 + 1, 4, 4, backgroundColor);
          spi->fillRect(x - 5, y + 3 + 10 + 1, 2, 6, backgroundColor);
          spi->fillTriangle(x, y + 3 + 10 + 6 + 2, x + 5, y + 3 + 10 + 6 + 2, x + 5, y + 3 + 10 + 3, backgroundColor);
        }
        else if (exp == Expression::Doubt)
        {
          spi->fillCircle(x + offsetX, y + offsetY, r, primaryColor);
          spi->fillCircle(x + offsetX, y + offsetY, r*0.9, backgroundColor);
          spi->fillCircle(x + offsetX, y + offsetY, r*0.6, TFT_LIGHTGRAY);
        }
        else if (exp == Expression::Sleepy)
        {
          spi->fillRect(x - 30, y - 5, 60, 7, primaryColor);
        }
        else if (exp == Expression::Neutral)
        {
          spi->fillCircle(x + offsetX, y + offsetY, r, primaryColor);
          spi->fillCircle(x + offsetX, y + offsetY, r*0.9, backgroundColor);
          spi->fillCircle(x + offsetX, y + offsetY, r*0.6, primaryColor);
        }
      }
      else
      {
        int x1 = x - r + offsetX;
        int y1 = y - 2 + offsetY;
        int w = r * 2;
        int h = 4;
        spi->fillRect(x1, y1, w, h, primaryColor);
      }
    }
  };

  /**
   * 口のクラス
   */
  class SacabambaspisMouth : public Drawable
  {
  private:
    uint16_t minWidth;
    uint16_t maxWidth;
    uint16_t minHeight;
    uint16_t maxHeight;

  public:
    SacabambaspisMouth() : SacabambaspisMouth(0, 0, 0, 0) {}
    SacabambaspisMouth(uint16_t minWidth, uint16_t maxWidth, uint16_t minHeight,
                       uint16_t maxHeight)
        : minWidth{minWidth},
          maxWidth{maxWidth},
          minHeight{minHeight},
          maxHeight{maxHeight} {}

    void draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx)
    {
      ColorPalette *cp = ctx->getColorPalette();
      uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : cp->get(COLOR_PRIMARY);
      uint16_t backgroundColor = ctx->getColorDepth() == 1 ? ERACER_COLOR : cp->get(COLOR_BACKGROUND);
      float breath = _min(1.0f, ctx->getBreath());
      float openRatio = ctx->getMouthOpenRatio();
      // Expression exp = ctx->getExpression();

      // 顔の色
      spi->fillRect(0, 0, 320, 120 + breath * 2, TFT_LIGHTGRAY);
      
      // 鼻
      // int nx = 160;
      // int ny = 115 + breath * 2;
      // spi->fillRoundRect(nx-25, ny-5, 15, 5, 2, primaryColor);
      // spi->fillRoundRect(nx+10, ny-5, 15, 5, 2, primaryColor);

      // 口
      int h = maxHeight* (1 - openRatio);
      int w = maxWidth * (1 - openRatio);
      int x1 = rect.getLeft();
      int y1 = rect.getTop() + h + breath * 2;
      int x2 = rect.getLeft() - w / 2;
      int y2 = rect.getTop();
      int x3 = rect.getLeft() + w / 2;
      int y3 = rect.getTop();

      spi->fillTriangle(x1, y1, x2, y2, x3, y3, primaryColor);
    }
  };

  /**
   * Faceクラスを継承したAA顔描画用のクラス
   */

  class SacabambaspisFace : public Face
  {
  public:
    SacabambaspisFace()
        : Face(new SacabambaspisMouth(40, 100, 0, 30), new BoundingRect(120, 160),
               new SacabambaspisEye(22, false), new BoundingRect(103, 70),
               new SacabambaspisEye(22, true), new BoundingRect(103, 250),
               new Eyeblow(12, 0, false), new BoundingRect(71, 50),
               new Eyeblow(12, 0, true), new BoundingRect(73, 270)) {}
  };

} // namespace m5avatar

#endif // FACES_SACABAMBASPISFACE_H_
