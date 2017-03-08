function [G] = draw_digraph(pfile, pttl, ont)
    %DRAW_DIGRAPH
    %
    %   [G] = DRAW_DIGRAPH(pfile, pttl, ont);
    %
    %       Draw an ontology as a directed graph.
    %
    % Input
    % -----
    % [char]
    % pfile:    The filename of the plot.
    %           Note that the file extension must be either 'png' or 'eps'.
    %           default: 'png'
    %
    % [char]
    % pttl:     The title of the plot.
    %
    % [struct]
    % ont:      An ontology structure. See pfp_ontbuild.m
    %
    % Output
    % ------
    % [digraph]
    % G:    A directed graph object.
    %
    % Dependency
    % ----------
    % [>] embed_canvas.m
    %
    % See also
    % --------
    % [>] pfp_ontbuill.m

    % check inputs {{{
    if nargin ~= 3
        error('draw_digraph:InputCount', 'Expected 3 inputs.');
    end

    % pfile
    validateattributes(pfile, {'char'}, {'nonempty'}, '', 'pfile', 1);
    [p, f, e] = fileparts(pfile);
    if isempty(e)
        e = '.png';
    end
    ext = validatestring(e, {'.eps', '.png'}, '', 'pfile', 1);
    if strcmp(ext, '.eps')
        device_op = '-depsc';
    elseif strcmp(ext, '.png')
        device_op = '-dpng';
    end

    % pttl
    validateattributes(pttl, {'char'}, {}, '', 'pttl', 2);

    % ont
    validateattributes(ont, {'struct'}, {'nonempty'}, '', 'ont', 3);
    % }}}

    % plot and save {{{
    G = digraph(ont.DAG');
    h = figure('Visible', 'off');
    H = plot(G);
    layout(H, 'layered');
    ax = gca;
    ax.Title.String = pttl;
    ax.YTick = [];
    ax.XTick = [];

    embed_canvas(h, 5, 4);
    print(h, pfile, device_op, '-r300');
    % }}}
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Tue 07 Mar 2017 12:30:47 PM E
