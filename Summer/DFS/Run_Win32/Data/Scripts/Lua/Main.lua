



function Update(ds)
    Write("hello");
end

function Render()
    --DrawCircle(0,0,20, true);
    DrawAABB2(-10, -10, 10, 10, true);
end

player = {
    position = {
    x = 32.5, y = 20.0
    },
    filename = "sprite.png",
    HP = 300
}