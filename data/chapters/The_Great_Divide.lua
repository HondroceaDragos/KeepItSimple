return {
    {
        id = 1,
        ["preamble"] = [=[[JASON] How could you do this to us?]=],
        choices = {
            {text = "What do you want from me?", goingTo = 2},
            {text = "Oh pluh-ease...", goingTo = 3}
        }
    },
    {
        id = 2,
        ["preamble"] = [=[[YOU] What do you want from me? To apologize? I'm [c: red, s: bold]fucking sorry[/], then! Here you go!]=],
        choices = {}
    },
    {
        id = 3,
        ["preamble"] = [=[[YOU] Oh, grow up, you fucking baby!]=],
        choices = {
            {text = "Keep going", goingTo = 4},
            {text = "Stop", goingTo = 5}
        }
    },
    {
        id = 4,
        ["preamble"] = [=[[YOU] You never did anything in your whole pathetic life. Now I have to bail you out! AGAIN?!]=],
        choices = {}
    },
    {
        id = 5,
        ["preamble"] = [=[You stare him down. Disgusting...]=],
        choices = {
            {text = "Get out", goingTo = 6},
            {text = "Apologize?", goingTo = 7}
        }
    },
    {
        id = 6,
        ["preamble"] = [=[You start walking away from Jason. Slowly. Each step heavier than the last.]=],
        choices = {{text = "", goingTo = 8}}
    },
    {
        id = 7,
        ["preamble"] = [=[[YOU] I'm sorry, little one... :(]=],
        choices = {{text = "", goingTo = 8}}
    },
    {
        id = 8,
        ["preamble"] = [=[Fin.]=],
        choices = {}
    },
}