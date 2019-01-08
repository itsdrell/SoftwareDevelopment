

--red = Rgba.new(255,0,0,255);

function Update(ds)
    --Write("hello");
    --red:PrintColor();
end

function Render()
    --DrawCircle(0,0,20, true);
    --red = Rgba.new(255,0,0,255);
    --DrawAABB2(-10, -10, 10, 10, true, red);
end

player = {
    position = {
    x = 32.5, y = 20.0
    },
    filename = "sprite.png",
    HP = 300,
    color = red
}