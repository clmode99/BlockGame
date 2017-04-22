GameAppTemplate(DxLib)


Ver.1.02  2016/06/15
  [修正]
  ・Game/GameMain.h および GameMain.cpp
    関数名の変更
    Initialize() ⇒ InitializeGame()
    Update()     ⇒ UpdateGame()
    Render()     ⇒ RenderGame()
    Finalize()   ⇒ FinalizeGame()

  ・Libralies/SpriteStudio/SpriteStudioPlayer.cpp
    描画周りの関数を修正


Ver.1.01  2016/04/15
  ・Visual Studio 2015に対応 (takaki)

  [修正]
  ・Libralies/SpriteStudio/SpriteStudioPlayer.cpp
    追加 ⇒ #include <memory>



Ver.1.00  2015/06/11
  ・ゲームアプリ制作用テンプレートの公開 (takaki)
