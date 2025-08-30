#!/usr/bin/env bash

_bin_graph_completion() {
    local arg_opts nonarg_opts
    arg_opts=(
        -m --mode
        -w --width
        -z --zoom
        --block-size
        --offset-start --offset-end
        --output-format
        --transform-squares
    )
    nonarg_opts=(
        -h --help
        --list-modes
        --list-output-formats
    )

    # If the previous option ('$3') is a redirector, show the default file
    # completion.
    if [[ "$3" == '2>' || "$3" == '>' || "$3" == '<' ]]; then
        compopt -o bashdefault -o default
        return
    fi

    # If the previous option expected an extra parameter, don't show completion.
    for arg in "${arg_opts[@]}"; do
        if [ "$3" = "$arg" ]; then
            return
        fi
    done

    if [[ "$2" = -* ]]; then
        # If the current option ('$2') starts with a dash, return (in
        # '$COMPREPLY') the possible completions for the current option using
        # 'compgen'.
        mapfile -t COMPREPLY < <(compgen -W "${arg_opts[*]} ${nonarg_opts[*]}" -- "$2")
    else
        # Otherwise, show the default file completion.
        compopt -o bashdefault -o default
    fi
}

complete -F _bin_graph_completion bin-graph
